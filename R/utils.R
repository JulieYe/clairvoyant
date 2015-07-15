`%||%` <- function(x, y) if (is.null(x)) y else x

#' Write raw model and feature files to separate config files for xgboost framework.
#' @param filename_model character. The name of the raw model file.
#' @param filename_features character. The name of the raw features file.
#' @param filename_tree_prefix character. The prefix path of the config files of trees.
#' @param filename_features_effective character. The name of the effective features file.
#' @param rds_x character. RDS file name of test data set (features).
#' @param rds_y character. RDS file name of test data set (response).
#' @param filename_test_x character. The name of test data set (features).
#' @param filename_test_y character. The name of test data set (response).
#' @export
write_config_xgboost <- function(filename_model, filename_features, 
  filename_tree_prefix, filename_features_effective,
  rds_x, rds_y, filename_test_x, filename_test_y) {
  if (!file.exists(filename_model) ||
      !file.exists(filename_features) || 
      !file.exists(rds_x) || 
      !file.exists(rds_y)) {
    cat("Config files do not exist\n")
    return(False)
  }

  unlink(paste0(filename_tree_prefix, ".*"))

  content_model <- readLines(filename_model)
  # Clean up a line a little bit.
  content_model <- unname(vapply(content_model,
    function(s) gsub(",missing=(.*)$", "", gsub("^(\t+)", "", s)), character(1)))

  features <- c()
  tree_index <- -1
  for (line in content_model) {
    # Indicate a start of a new tree.
    if (grepl("^booster", line)) {
      tree_index <- as.integer(gsub("[^0-9]", "", line))
      next
    }
    # Make sure a specific tree is worked on.
    if (tree_index < 0) next
    filename_tree <- paste(filename_tree_prefix, as.character(tree_index), sep = ".")
    # Parse the line.
    node <- sub(":(.*)", "", line)
    rest <- sub("^[0-9]+:", "", line)
    if (grepl("^leaf", rest)) {
      feature <- "-1"
      split_value <- strsplit(rest, "=")[[1]][2]
      left_child <- "0"
    } else {
      rest <- gsub("(yes=|no=|missing=|f|\\[|\\])", "", rest)
      splits <- strsplit(rest, " ")[[1]]
      splits1 <- strsplit(splits[1], "<")[[1]]
      feature <- splits1[1]
      split_value <- splits1[2]
      left_child <- strsplit(splits[2], ",")[[1]][1]
    }
    # Write to config.
    cat(paste0(paste(node, feature, split_value, left_child, sep = ","), "\n"), 
      file = filename_tree, append = TRUE)
    # Append to feature indices set.
    if (!as.integer(feature) %in% features) {
      features <- sort(c(features, as.integer(feature)))
    }
  }

  content_features <- readLines(filename_features)
  unlink(filename_features_effective)
  for (line in content_features) {
    if (as.integer(strsplit(line, ":")[[1]][2]) %in% features) {
      cat(paste0(line, "\n"), file = filename_features_effective, append = TRUE)
    }
  }

  dir_trees <- stringr::str_replace(filename_tree_prefix, "tree$", "")
  if (!file.exists(dir_trees)) {
    cat("Tree config files do not exist\n")
    return(False)
  }

  cleanup_config_xgboost(dir_trees, filename_features_effective, features,
    rds_x, rds_y, filename_test_x, filename_test_y)

  TRUE
}

cleanup_config_xgboost <- function(dir_trees, filename_features, features,
  rds_x, rds_y, filename_test_x, filename_test_y) {

  content_features <- readLines(filename_features)
  unlink(filename_features)
  dict <- list()
  removes <- c()
  for (i in seq_along(content_features)) {
    line_splitted <- strsplit(content_features[i], ":")[[1]]
    dict[[line_splitted[2]]] <- as.character(i - 1)
    line_splitted[2] <- as.character(i - 1)
    cat(paste0(paste(line_splitted, collapse = ":"), "\n"), 
      file = filename_features, append = TRUE)
  }

  parallel::mclapply(list.files(dir_trees),
    function(f) {
      filename_tree <- file.path(dir_trees, f)
      lines <- readLines(filename_tree)
      unlink(filename_tree)
      for (line in lines) {
        line_splitted <- strsplit(line, ",")[[1]]
        if (as.integer(line_splitted[2]) > 0) {
          line_splitted[2] <- dict[[line_splitted[2]]]
          cat(paste0(paste(line_splitted, collapse = ","), "\n"), 
            file = filename_tree, append = TRUE)
        } else {
          cat(paste0(line, "\n"), file = filename_tree, append = TRUE)
        }
      }
    }, mc.cores = parallel::detectCores()
  )

  dat_x <- readRDS(rds_x)
  features <- vapply(features, function(x) x + 1, numeric(1))
  unlink(filename_test_x)
  for (i in 1:nrow(dat_x)) {
    cat(paste0(paste(dat[i, features], collapse = ","), "\n"), 
        file = filename_test_x, append = TRUE)
  }
  dat_y <- readRDS(rds_y)
  unlink(filename_test_y)
  cat(paste(dat_y, collapse = "\n"), file = filename_test_x, append = TRUE)

  invisible(NULL)
}

dual_scale_plot <- function(df, x = 'index', y1, y2, filepath ) {
  if(!missing(filepath)) png(filepath)
  stopifnot(c(x, y1, y2) %in% names(df))
  y1_text <- eval(substitute(y1))
  y2_text <- eval(substitute(y2))
  plot(df[[x]], df[[y1]], type="l", col="red", xlab = x, ylab = y1_text )
  par(new=TRUE)
  plot(df[[x]], df[[y2]],,type = "l", col = "blue", xaxt = "n", yaxt = "n", xlab = "", ylab = "")
  axis(4)
  mtext(y2_text, side = 4, line = -1)
  legend("topleft", col =c("red", "blue"), lty = 1, legend = c(y1_text, y2_text))
  if(!missing(filepath)) dev.off()
}
