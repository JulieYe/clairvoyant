#' Prepare data for financial analysis.
#' @param dir_data character. The directory of the csv data files. 
#' @param sep character. The separator of csv file. 
#' @param horizon integer. The prediction horizon in seconds.
#' @param subsample_freq integer. The daily sparsity of subsample frequency.
#' @param ticksize numeric. The tick size of the predicted instrument.
#' @param cap numeric. Cap in terms of standard deviation.
#' @param trainpct numeric. The percentage of training data set.
#' @param seed integer. Seed of randomized validation data set.
#' @param verbose logical. As name suggests.
#' @param debug logical. Whether datetime will be appended to the output of data frame.
#' @export
load_data <- function(dir_data, sep, horizon, 
  subsample_freq = 10, ticksize = 0.2, cap = Inf,
  trainpct = 0.9, seed = 29, 
  verbose = TRUE, debug = FALSE) {

  stopifnot(require(data.table))
  stopifnot(require(bit64))

  dat <- data.frame()
  filenames <- dir(dir_data)
  first <- TRUE
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
    if (is.finite(cap)) {
      if (isTRUE(verbose)) print("Cap is on")
      for (col_name in names(dat_i)) {
        cap_val <- cap * sd(dat_i[[col_name]])
          dat_i[dat_i[[col_name]] > cap_val, col_name] <- cap_val
          dat_i[dat_i[[col_name]] < -cap_val, col_name] <- -cap_val
      }
    }
    if (isTRUE(verbose)) print(paste0("Append ", nrow(dat_i), " by ", ncol(dat_i), " data frame"))
    if (isTRUE(debug)) dat_i <- cbind(dat_i, datetime = date_time)
    if (first) {
      dat <- rbind(dat, dat_i[seq(subsample_freq, nrow(dat_i), 
        by = subsample_freq), , drop = FALSE])
      first <- FALSE
    } else {
      dat <- rbind(dat, setNames(dat_i[seq(subsample_freq, nrow(dat_i), 
        by = subsample_freq), , drop = FALSE], names(dat)))
    }
    if (isTRUE(verbose)) print(paste0("Parsed ", filename))
  }
  cat("Response variable summary:\n")
  print(summary(round(dat$Change / ticksize)))
  set.seed(seed)
  id_tr <- caret::createDataPartition(factor(round(dat$Change / ticksize)), 
    p = trainpct, list = FALSE, times = 1)[, 1]
  list(dat_tr = dat[id_tr, ], dat_ts = dat[-id_tr, ]) 
}

#' Evaluate the model on training data set and output the performance.
#' @param dat_tr data.frame.
#' @param dat_ts data.frame.
#' @param n_trees interger. Number of trees.
#' @param eta numeric. Shrinkage parameter.
#' @param max_depth integer. Number of maximum depth of tree.
#' @param subsample numeric. Percentage of row sampling.
#' @param colsample_bytree numeric. Percentage of column sampling.
#' @param nthread integer. Number of threads.
#' @param output logical. Whether to save the model as texts.
#' @param output_path character. Path to output directory.
#' @param fname_model character. Text file to save the model.
#' @param fname_model_bin character. Binary file to save the model.
#' @param fname_feature character. Text file to save the features.
#' @param fname_rds_x character. RDS file name of test data set (features).
#' @param fname_rds_y character. RDS file name of test data set (response).
#' @param fname_grid_search character. Output for grid searching.
#' @param dep_var character. Name of dependent variable.
#' @param n_test_samples integer. Number of samples for unit test.
#' @param dep_var_range numeric. Range of dep_var for R2.
#' @export
evaluate_model <- function(dat_tr, dat_ts, n_trees = 500, eta = 0.01, max_depth = 7, 
    subsample = 0.75, colsample_bytree = 0.75, nthread, output = FALSE, output_path, 
    fname_model = "xgboost.model", fname_model_bin = "xgboost.model.bin", 
    fname_feature = "xgboost.features", fname_rds_x = "tree.input.rds", 
    fname_rds_y = "tree.output.rds", fname_grid_search, 
    dep_var = "Change", n_test_samples = 5000, dep_var_range) {

  stopifnot(require(xgboost))

  if (missing(dep_var_range) || 
      !is.vector(dep_var_range)) {
    dep_var_range <- c(seq(0, 10, by = 2), Inf)
  }

  id_0_var <- which(vapply(dat_tr, sd, numeric(1)) == 0)
  if (length(id_0_var) > 0) {
    print(paste0(paste(colnames(dat_tr)[id_0_var], collapse = "\n"), " are removed"))
    dat_tr <- dat_tr[, -id_0_var]
    dat_ts <- dat_ts[, -id_0_var]
  }

  dtrain <- xgboost::xgb.DMatrix(as.matrix(dat_tr[, !colnames(dat_tr) %in% c("Change")]), 
    label = dat_tr$Change, missing = NA)
  dtest <- xgboost::xgb.DMatrix(as.matrix(dat_ts[, !colnames(dat_ts) %in% c("Change")]), 
    label = dat_ts$Change, missing = NA)

  header_R2_decompose <- vapply(1:(length(dep_var_range)-1), 
    function(i) paste0("R2_", dep_var_range[i], "_", dep_var_range[i+1]), 
    character(1))
  value_R2_decompose <- lapply(1:(length(dep_var_range)-1), 
    function(i) c(dep_var_range[i], dep_var_range[i+1]))

  if (length(n_trees) > 1 || length(eta) > 1 || length(max_depth) > 1 ||
      length(subsample) > 1 || length(colsample_bytree) > 1) {
    par_grid <- expand.grid(list(n_trees = n_trees, eta = eta, max_depth = max_depth, 
      subsample = subsample, colsample_bytree = colsample_bytree))

    r2 <- rep(NA, nrow(par_grid))
    r2_decompose <- rep(NA, length(value_R2_decompose))

    header = paste(c("i", colnames(par_grid), "R2_tr", "R2_ts", 
      header_R2_decompose, "optimality\n"), collapse = " ")
    if (missing(fname_grid_search)) {
      cat(header)
    } else {
      cat(header, file = file.path(output_path, fname_grid_search), append = TRUE)
    }
    for (i in seq_len(nrow(par_grid))) {
      param <- list(eta = par_grid[i, "eta"],
        max.depth = par_grid[i, "max_depth"], 
        subsample = par_grid[i, "subsample"], 
        colsample_bytree = par_grid[i, "colsample_bytree"],
        silent = 1, objective='reg:linear',
        nthread = ifelse(missing(nthread), parallel::detectCores(), 1),
        base_score = 0)
      bst <- xgboost::xgb.train(param, dtrain, par_grid[i, "n_trees"])
      ptrain <- xgboost::predict(bst, dtrain)
      ptest <- xgboost::predict(bst, dtest)
      r2_train <- 1 - sum((dat_tr$Change - ptrain) ^ 2) / sum(dat_tr$Change ^ 2)
      r2[i] <- 1 - sum((dat_ts$Change - ptest) ^ 2) / sum(dat_ts$Change ^ 2)

      for (j in seq_along(value_R2_decompose)) {
        indices <- which(abs(dat_ts$Change) >= value_R2_decompose[[j]][1] & 
            abs(dat_ts$Change) < value_R2_decompose[[j]][2])
        if (length(indices) > 0) {
          var_noise <- sum((dat_ts$Change[indices] - ptest[indices]) ^ 2)
          var_total <- sum(dat_ts$Change[indices] ^ 2)
          r2_decompose[j] <- 1 - var_noise / var_total
        }
      }

      line <- paste0(paste(c(i, par_grid[i, , drop = FALSE], 
        round(r2_train, 4), round(r2[i], 4), 
        vapply(r2_decompose, function(v) round(v, 4), numeric(1))), collapse = "\t"), 
        ifelse(r2[i] >= max(r2, na.rm = TRUE), "\t*\n", "\n"))
      if (missing(fname_grid_search)) {
        cat(line)
      } else {
        cat(line, file = file.path(output_path, fname_grid_search), append = TRUE)
      }
    }
    
    idx_optimal <- which.max(r2)
    if (length(idx_optimal) == 0) {
      stop("evaluate_model failed, all R2 is missing", call. = FALSE)
    }

    n_trees <- par_grid[idx_optimal, "n_trees"]
    eta <- par_grid[idx_optimal, "eta"]
    max_depth <- par_grid[idx_optimal, "max_depth"]
    subsample <- par_grid[idx_optimal, "subsample"]
    colsample_bytree <- par_grid[idx_optimal, "colsample_bytree"]
    line <- paste0("n_trees: ", n_trees, ", eta: ", eta,
      ", max_depth: ", max_depth, ", subsample: ", subsample,
      ", colsample_bytree: ", colsample_bytree, "\n")
    if (missing(fname_grid_search)) {
      cat(line)
    } else {
      cat(line, file = file.path(output_path, fname_grid_search), append = TRUE)
    }
  } else {
    par_grid <- list(n_trees = n_trees, eta = eta, max_depth = max_depth, 
      subsample = subsample, colsample_bytree = colsample_bytree)

    header = paste(c("i", names(par_grid), "R2_tr", "R2_ts", 
      header_R2_decompose, "\n"), collapse = " ")
    if (missing(fname_grid_search)) {
      cat(header)
    } else {
      cat(header, file = file.path(output_path, fname_grid_search), append = TRUE)
    }

    param <- list(eta = eta, max.depth = max_depth, subsample = subsample, 
        colsample_bytree = colsample_bytree, silent = 1, objective='reg:linear',
        nthread = ifelse(missing(nthread), parallel::detectCores(), 1), base_score = 0)
    bst <- xgboost::xgb.train(param, dtrain, n_trees)

    ptrain <- xgboost::predict(bst, dtrain)
    ptest <- xgboost::predict(bst, dtest)

    r2_train <- 1 - sum((dat_tr$Change - ptrain) ^ 2) / sum(dat_tr$Change ^ 2)
    r2_test <- 1 - sum((dat_ts$Change - ptest) ^ 2) / sum(dat_ts$Change ^ 2)

    r2_decompose <- rep(NA, length(value_R2_decompose))

    for (j in seq_along(value_R2_decompose)) {
      indices <- which(abs(dat_ts$Change) >= value_R2_decompose[[j]][1] & 
          abs(dat_ts$Change) < value_R2_decompose[[j]][2])
      if (length(indices) > 0) {
        var_noise <- sum((dat_ts$Change[indices] - ptest[indices]) ^ 2)
        var_total <- sum(dat_ts$Change[indices] ^ 2)
        r2_decompose[j] <- 1 - var_noise / var_total
      }
    }

    line <- paste0(paste(c(1, unlist(par_grid), 
      round(r2_train, 4), round(r2_test, 4), 
      vapply(r2_decompose, function(v) round(v, 4), numeric(1))), collapse = "\t"), "\n")
    if (missing(fname_grid_search)) {
      cat(line)
    } else {
      cat(line, file = file.path(output_path, fname_grid_search), append = TRUE)
    }
  }

  # Write to config file.
  if (isTRUE(output)) {
    dat <- rbind(dat_tr, dat_ts)
    dfull <- xgboost::xgb.DMatrix(as.matrix(dat[, !colnames(dat) %in% c("Change")]), 
      label = dat$Change, missing = NA)
    bst <- xgboost::xgb.train(param, dfull, n_trees)
    pfull  <- xgboost::predict(bst, dfull)
    line <- paste0("Full-sample R2: ", 
      1 - sum((dat$Change - pfull) ^ 2) / sum(dat$Change ^ 2), "\n")
    if (missing(fname_grid_search)) {
      cat(line)
    } else {
      cat(line, file = file.path(output_path, fname_grid_search), append = TRUE)
    }

    # Save the model in text format.
    unlink(file.path(output_path, fname_model))
    xgboost::xgb.dump(bst, file.path(output_path, fname_model))

    # Save the model in binary format.
    unlink(file.path(output_path, fname_model_bin))
    xgboost::xgb.save(bst, file.path(output_path, fname_model_bin))

    # Save the features
    names_to_save <- colnames(dat[, !colnames(dat) %in% c("Change")]) 
    unlink(file.path(output_path, fname_feature))
    for (i in seq_along(names_to_save)) {
      cat(paste0(names_to_save[i], ":", i - 1, "\n"), 
          file = file.path(output_path, fname_feature), append = TRUE)
    }

    # Save the unit test data randomly
    n_test_samples <- min(n_test_samples, nrow(dat))
    indices <- sample(nrow(dat), n_test_samples)
    saveRDS(dat[indices, !colnames(dat) %in% c("Change"), drop = FALSE], 
        file.path(output_path, fname_rds_x))
    saveRDS(pfull[indices], file.path(output_path, fname_rds_y))
  }

  invisible(NULL)
}
