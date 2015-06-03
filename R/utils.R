`%||%` <- function(x, y) if (is.null(x)) y else x

formulify <- function(vars) {
  formula(paste(collapse = " + ", c('~', vars)), env = baseenv())
}
