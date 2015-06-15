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

  stopifnot(require(data.table))
  stopifnot(require(bit64))

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
  list(dat_tr = dat[id_tr, ], dat_ts = dat[-id_tr, ]) 
}

#' Evaluate the model on training data set and output the performance.
#' @param dat_tr data.frame.
#' @param dat_ts data.frame.
#' @param dep_var character. Name of dependent variable.
#' @param n_trees interger. Number of trees.
#' @export
evaluate_model <- function(dat_tr, dat_ts, dep_var = "Change", n_trees = 500) {

  id_0_var <- which(vapply(dat_tr, sd, numeric(1)) == 0)
  if (length(id_0_var) > 0) {
    print(paste0(paste(colnames(dat_tr)[id_0_var], collapse = "\n"), " are removed"))
    dat_tr <- dat_tr[, -id_0_var]
    dat_ts <- dat_ts[, -id_0_var]
  }

  dtrain <- xgboost::xgb.DMatrix(as.matrix(dat_tr[, !colnames(dat_tr) %in% c("Change")]), 
    label = dat_tr$Change)
  dtest <- xgboost::xgb.DMatrix(as.matrix(dat_ts[, !colnames(dat_ts) %in% c("Change")]), 
    label = dat_ts$Change)

  param <- list(max.depth=6, eta=0.001, nthread = 4, silent=1, objective='reg:linear',
        subsample = 0.5, colsample_bytree = 0.5)
  bst <- xgboost::xgb.train(param, dtrain, n_trees)
  ptrain  <- xgboost::predict(bst, dtrain, outputmargin=TRUE)
  ptest  <- xgboost::predict(bst, dtest, outputmargin=TRUE)
  cat(paste0("In-sample R2: ", 1 - sum( (dat_tr$Change - ptrain) ^ 2) / sum(dat_tr$Change ^ 2)))
  cat(paste0("Out-of-sample R2 ", 1 - sum( (dat_ts$Change - ptest) ^ 2) / sum(dat_ts$Change ^ 2)))
}
