#' Prepare data for financial analysis.
#' @param dir_data character. The directory of the csv data files. 
#' @param sep character. The separator of csv file. 
#' @param horizon integer. The prediction horizon in seconds.
#' @param subsample_freq integer. The daily sparsity of subsample frequency.
#' @param ticksize numeric. The tick size of the predicted instrument.
#' @param trainpct numeric. The percentage of training data set.
#' @param seed integer. Seed of randomized validation data set.
#' @export
load_data <- function(dir_data, sep, horizon, 
  subsample_freq = 10, ticksize = 0.2, trainpct = 0.8, seed = 29, 
  verbose = TRUE, debug = FALSE) {

  stopifnot(require(data.table))
  stopifnot(require(bit64))

  dat <- data.frame()
  filenames <- dir(dir_data)
  #first <- TRUE
  for (filename in filenames) {
    if (isTRUE(verbose)) print(paste0("Parsing ", filename))
    dat_i <- data.table::fread(paste(dir_data, filename, sep = "/"), sep = sep)
    class(dat_i) <- "data.frame"
    px_to <- grep(paste0("MidPoint<>\\(.*\\)_", horizon * 1000, "$"), colnames(dat_i))
    if (length(px_to) != 1) {
      if (isTRUE(verbose)) print("Did not get the specific horizin to predict on")
      next
    }
    instrument_predict <- strsplit(strsplit(colnames(dat_i)[px_to], 
      split = "\\(")[[1]][2], "\\)")[[1]][1]
    px_from <- grep(paste0("MidPoint<>\\(", instrument_predict, "\\)_0"), colnames(dat_i))
    if (length(px_from) != 1) {
      if (isTRUE(verbose)) print("Did not get the unique instrument to predict on")
      next
    }
    px_from <- dat_i[, px_from]
    px_to <- dat_i[, px_to]
    dat_i <- dat_i[, -grep("MidPoint<>", colnames(dat_i))]
    if (isTRUE(debug)) date_time <- dat_i$Datetime
    dat_i <- dat_i[, grep("^Feature_.*_0$", colnames(dat_i))]
    dat_i <- cbind(dat_i, "Change" = px_to - px_from)
    # Cap to 5 sd
    for (col_name in names(dat_i)) {
      cap <- 3 * sd(dat_i[[col_name]])
      dat_i[dat_i[[col_name]] > cap, col_name] <- cap
      dat_i[dat_i[[col_name]] < -cap, col_name] <- -cap
    }
    if (isTRUE(verbose)) print(paste0("Append ", nrow(dat_i), " by ", ncol(dat_i), " data frame"))
    #if (first) {
      if (isTRUE(debug)) dat_i <- cbind(dat_i, datetime = date_time)
      dat <- rbind(dat, dat_i[seq(subsample_freq, nrow(dat_i), 
        by = subsample_freq), , drop = FALSE])
    #} else {
    #  dat <- rbind(dat, setNames(dat_i[seq(subsample_freq, nrow(dat_i), 
    #    by = subsample_freq), , drop = FALSE], names(dat)))
    #}
    #first <- FALSE
    if (isTRUE(verbose)) print(paste0("Parsed ", filename))
  }
  set.seed(seed)
  id_tr <- caret::createDataPartition(factor(round(dat$Change / ticksize)), 
    p = trainpct, list = FALSE, times = 1)[, 1]
  list(dat_tr = dat[id_tr, ], dat_ts = dat[-id_tr, ]) 
}

#' Evaluate the model on training data set and output the performance.
#' @param dat_tr data.frame.
#' @param dat_ts data.frame.
#' @param n_trees interger. Number of trees.
#' @param output logical. Whether to save the model as texts.
#' @param model_filename character. Text file to save the model.
#' @param feature_filename character. Text file to save the features.
#' @param dep_var character. Name of dependent variable.
#' @export
evaluate_model <- function(dat_tr, dat_ts, n_trees = 500, output = FALSE,
  model_filename, feature_filename, dep_var = "Change") {

  stopifnot(require(xgboost))

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

  param <- list(max.depth=7, eta=0.001, nthread = 4, silent=1, objective='reg:linear',
        subsample = 0.5, colsample_bytree = 0.5)
  bst <- xgboost::xgb.train(param, dtrain, n_trees)
  ptrain  <- xgboost::predict(bst, dtrain, outputmargin=TRUE)
  ptest  <- xgboost::predict(bst, dtest, outputmargin=TRUE)
  cat(paste0("In-sample R2: ", 
    1 - sum( (dat_tr$Change - ptrain) ^ 2) / sum(dat_tr$Change ^ 2), "\n"))
  cat(paste0("Out-of-sample R2 ", 
    1 - sum( (dat_ts$Change - ptest) ^ 2) / sum(dat_ts$Change ^ 2), "\n"))
  if (isTRUE(output)) {
    dat <- rbind(dat_tr, dat_ts)
    dfull <- xgboost::xgb.DMatrix(as.matrix(dat[, !colnames(dat) %in% c("Change")]), 
      label = dat$Change)
    bst <- xgboost::xgb.train(param, dfull, n_trees)
    pfull  <- xgboost::predict(bst, dfull, outputmargin=TRUE)
    cat(paste0("Full-sample R2: ", 
      1 - sum( (dat$Change - pfull) ^ 2) / sum(dat$Change ^ 2), "\n"))

    # Save the model
    xgboost::xgb.dump(bst, model_filename)

    # Save the features
    names_to_save <- colnames(dat[, !colnames(dat) %in% c("Change")]) 
    for (i in seq_along(names_to_save)) {
      if (i == 1) {
        cat(paste0(names_to_save[i], ":", i - 1, "\n"), file = feature_filename, append = FALSE)
      } else {
        cat(paste0(names_to_save[i], ":", i - 1, "\n"), file = feature_filename, append = TRUE)
      }
    }
  }
  invisible(NULL)
}
