`%||%` <- function(x, y) if (is.null(x)) y else x

convert2config <- function(filename_model, filename_features, 
  filename_tree_prefix, filename_features_effective) {
  if (!file.exists(filename_model) ||
      !file.exists(filename_features)) {
    return(False)
  }

  content_model <- readLines(filename_model)
  # Clean up a line a little bit.
  content_model <- unname(vapply(content_model,
    function(s) gsub(",missing=(.*)$", "", gsub("^(\t+)", "", s)), character(1)))

  features <- c()
  tree_index <- 0
  for (line in content_model) {
    # Indicate a start of a new tree.
    if (grepl("^booster", line)) {
      tree_index <- as.integer(gsub("[^0-9]", "", line)) + 1  
      next
    }
    # Make sure a specific tree is worked on.
    if (tree_index == 0) next
    filename_tree <- paste(filename_tree_prefix, as.character(tree_index), sep = ".")
    # Parse the line.
    node <- sub(":(.*)", "", line)
    rest <- gsub("([[:alpha:]]|=|\\[|\\])", "", sub("^[0-9]+:", "", line))
    if (grepl("leaf", rest)) {
      feature <- -1
      split_value <- strsplit(rest, "=")[[1]][2]
      left_child <- 0
    } else {
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
  for (line in content_features) {
    if (as.integer(strsplit(line, ":")[[1]][2]) %in% features) {
      cat(paste0(line, "\n"), file = filename_features_effective, append = TRUE)
    }
  }
  TRUE
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
