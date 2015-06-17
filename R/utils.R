`%||%` <- function(x, y) if (is.null(x)) y else x

dual_scale_plot <- function(df, x = 'date', y1, y2, filepath ) {
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
