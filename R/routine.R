#' Prepare data for financial analysis.
#' @param filenames an atomic vector of character. The name of the data csv file. 
#' @param sep character. The separator of csv file. 
#' @param instrument_predict character. The instrument to predict the price change in the future.
#' @param instruments_train an atomic vector of character. The signaling instruments.
#' @param horizon integer. The prediction horizon in seconds.
#' @param ticksize numeric. The tick size of the predicted instrument.
#' @param trainpct numeric. The percentage of training data set.
#' @param seed integer. Seed of randomized validation data set.
#' @export
prepare_data <- function(filenames, sep, instrument_predict, instruments_train, horizon,
  ticksize = 0.2, trainpct = 0.8, seed = 29) {

  stopifno(require(bit64))

  dat <- data.frame()
  for (filename in filenames) {
    dat_i <- data.table::fread(filename, sep = sep)
    class(dat_i) <- "data.frame"
    px_from <- grep(paste0("MidPoint<>\\(", instrument_predict, "\\)_0"), colnames(dat_i))
    px_to <- grep(paste0("MidPoint<>\\(", instrument_predict, "\\)_", horizon * 1000), 
      colnames(dat_i))
    if (length(px_from) != 1 ||
        length(px_to) != 1) {
      print("Did not get the unique instrument to predict on")
      next
    }
    px_from <- dat_i[, px_from]
    px_to <- dat_i[, px_to]
    dat_i <- dat_i[, -grep("MidPoint<>", colnames(dat_i))] 
    dat_i <- dat_i[, grep("^Feature_.*_0$", colnames(dat_i))]
    dat_i <- cbind(dat_i, "Change" = px_to - px_from)
    dat <- rbind(dat, dat_i)
  }
  set.seed(seed)
  id_tr <- caret::createDataPartition(factor(round(dat$Change / ticksize)), 
    p = trainpct, list = FALSE, times = 1)[, 1]
  list(data_tr = dat[id_tr, ], data_ts = dat[-id_tr, ]) 
}


#' Train the model on training data set and output the performance.
#' @param dat_tr data.frame.
#' @param dat_ts data.frame.
#' @param dep_var character. Name of dependent variable.
#' @export
train <- function(dat_tr, dat_ts, dep_var = "Change") {

  dat_tr <- dat_tr[, colnames(dat_tr), drop = FALSE]
  colnames(dat_tr) <- vapply(colnames(dat_tr),
    function(x) gsub("[<>()|]", "_", x), character(1), USE.NAMES = FALSE)
  colnames(dat_ts) <- vapply(colnames(dat_ts),
    function(x) gsub("[<>()|]", "_", x), character(1), USE.NAMES = FALSE)

  factor_cols <- Filter(function(x) is.factor(dat_tr[[x]]), 
    setdiff(colnames(dat_tr), dep_var))

  contrasts <- sapply(factor_cols, 
    function(x) contr.treatment(levels(dat_tr[, x]), 
    contrasts = FALSE), USE.NAMES = TRUE)

  options_na_action_restore <- options(na.action = 'na.pass')
  on.exit(options(options_na_action_restore), add = TRUE)

  form <- formulify(setdiff(colnames(dat_tr), dep_var))

  data_tr <- model.matrix(form, data = dat_tr, contrasts.arg = contrasts)[, -1]
  data_ts <- model.matrix(form, data = dat_ts, contrasts.arg = contrasts)[, -1]
  label_tr <- dat_tr[[dep_var]]
  label_ts <- dat_ts[[dep_var]]

  dtrain <- xgboost::xgb.DMatrix(data_tr, label = label_tr, missing = NA)
  dtest <- xgboost::xgb.DMatrix(data_ts, label = label_ts, missing = NA)

  watchlist <- list(eval = dtest, train = dtrain)

  param <- list(max.depth=7,eta=0.001,nthread = 4, silent=1,objective='reg:linear',
        subsample = 0.5, colsample_bytree = 0.5)
  bst <- xgb.train( param, dtrain, 6000, watchlist )
  ptrain  <- predict(bst, dtrain, outputmargin=TRUE)
  ptest  <- predict(bst, dtest, outputmargin=TRUE)
  print(1 - sum( (label_tr - ptrain) ^ 2) / sum(label_tr ^ 2))
  print(1 - sum( (label_ts - ptest) ^ 2) / sum(label_ts ^ 2))
}
