`%||%` <- function(x, y) if (is.null(x)) y else x

#' Write raw model and feature files to separate config files for xgboost framework.
#' @param input_path character. The path to input directory.
#' @param fname_model character. The name of the raw model file.
#' @param fname_features character. The name of the raw features file.
#' @param fname_rds_x character. RDS file name of test data set (features).
#' @param fname_rds_y character. RDS file name of test data set (response).
#' @param output_path character. The path to output directory.
#' @param fname_tree_prefix character. The prefix path of the config files of trees.
#' @param fname_features_effective character. The name of the effective features file.
#' @param fname_test_x character. The name of test data set (features).
#' @param fname_test_y character. The name of test data set (response).
#' @export
write_config_xgboost <- function(input_path, fname_model = "xgboost.model", 
  fname_features = "xgboost.features", fname_rds_x = "tree.input.rds", 
  fname_rds_y = "tree.output.rds", output_path, fname_tree_prefix = "trees/tree",  
  fname_features_effective = "xgboost.features.in.use",
  fname_test_x = "tree.input", fname_test_y = "tree.output") {

  fname_model <- file.path(input_path, fname_model)
  fname_features <- file.path(input_path, fname_features)
  fname_rds_x <- file.path(input_path, fname_rds_x)
  fname_rds_y <- file.path(input_path, fname_rds_y)
  fname_tree_prefix <- file.path(output_path, fname_tree_prefix)
  fname_features_effective <- file.path(output_path, fname_features_effective)
  fname_test_x <- file.path(output_path, fname_test_x)
  fname_test_y <- file.path(output_path, fname_test_y)

  if (!all(file.exists(input_path, fname_model, fname_features, 
          fname_rds_x, fname_rds_y, output_path))) {
    cat("Config files or paths do not exist\n")
    return(False)
  }

  float_bounds <- float_minmax()

  unlink(paste0(fname_tree_prefix, ".*"))

  content_model <- readLines(fname_model)
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
    filename_tree <- paste(fname_tree_prefix, as.character(tree_index), sep = ".")
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
    split_value <- as.character(bound_minmax(split_value, float_bounds))
    # Write to config.
    cat(paste0(paste(node, feature, split_value, left_child, sep = ","), "\n"), 
      file = filename_tree, append = TRUE)
    # Append to feature indices set.
    if (!as.integer(feature) %in% features) {
      features <- sort(c(features, as.integer(feature)))
    }
  }

  content_features <- readLines(fname_features)
  unlink(fname_features_effective)
  for (line in content_features) {
    if (as.integer(strsplit(line, ":")[[1]][2]) %in% features) {
      cat(paste0(line, "\n"), file = fname_features_effective, append = TRUE)
    }
  }

  dir_trees <- stringr::str_replace(fname_tree_prefix, "tree$", "")
  if (!file.exists(dir_trees)) {
    cat("Tree config files do not exist\n")
    return(False)
  }

  cleanup_config_xgboost(dir_trees, fname_features_effective, features,
    fname_rds_x, fname_rds_y, fname_test_x, fname_test_y)

  TRUE
}

cleanup_config_xgboost <- function(dir_trees, fname_features, features,
  fname_rds_x, fname_rds_y, fname_test_x, fname_test_y) {

  content_features <- readLines(fname_features)
  unlink(fname_features)
  dict <- list()
  removes <- c()
  for (i in seq_along(content_features)) {
    line_splitted <- strsplit(content_features[i], ":")[[1]]
    dict[[line_splitted[2]]] <- as.character(i - 1)
    line_splitted[2] <- as.character(i - 1)
    cat(paste0(paste(line_splitted, collapse = ":"), "\n"), 
      file = fname_features, append = TRUE)
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

  dat_x <- readRDS(fname_rds_x)
  features <- vapply(features, function(x) x + 1, numeric(1))
  unlink(fname_test_x)
  for (i in 1:nrow(dat_x)) {
    cat(paste0(paste(
    vapply(dat_x[i, features], bound_minmax, bounds = float_bounds, numeric(1)), 
    collapse = ","), "\n"), 
        file = fname_test_x, append = TRUE)
  }
  dat_y <- readRDS(fname_rds_y)
  unlink(fname_test_y)
  cat(paste(dat_y, collapse = "\n"), file = fname_test_y, append = TRUE)

  invisible(NULL)
}

float_minmax <- function() {
  Rcpp::cppFunction("float float_min() { return std::numeric_limits<float>::min(); }")
  Rcpp::cppFunction("float float_max() { return std::numeric_limits<float>::max(); }")
  # I am not really interested in handling the boundaries.
  c(float_min() * 10, float_max() / 10)
}

bound_minmax <- function(x, bounds) {
  if (missing(bounds)) bounds <- float_minmax()
  if (x > 0) min(bounds[2], max(bounds[1], x))  
  else if (x < 0) max(-bounds[2], min(-bounds[1], x))  
  else x
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
