#' Convert physical installment number to paid principal percentage
#' @param i numeric. Index of monthly installment.
#' @param r numeric. Annualized interest rate in decimal.
#' @param n numeric. Total number of installments (term) in month.
#' @export
p_to_q <- function(i, r, n) {
  if (abs(r) <= 12 * .Machine$double.eps) {
    return(i / n)
  } else {
    1 - (1 + r / 12) ^ i - (1 - (1 + r / 12) ^ i) / (1 - 1 / (1 + r / 12) ^ n)
  }
}

#' Generate remaining principal percentage vector.
#' @param r numeric. Annualized interest rate in decimal.
#' @param n numeric. Total number of installments (term) in month.
#' @export
generate_q_time <- function(r, n) {
  vapply(1:n, function(i) p_to_q(i, r, n), numeric(1))
}

#' Calibrate APR based on a survival curve.
#' @param S function or numeric atomic vector. Survival curve.
#' @param b numeric. Flat forward curve reflecting the cost of funding.
#' @param n numeric. Total number of installments (term) in month.
#' @param version numeric. Version: 1 - q time; 2 - p time; 3 - number of installment.
#' @export
calibrate_apr <- function(S, b = 0.1, n = 12, version = 1, ...) {
  if (version %in% c(1,2)) {
    if (!is.function(S)) return(NA)
    fcn2optimise <- function(r) {
      I <- if (abs(r) <= 10 * .Machine$double.eps) 1 / n
           else r / 12 / (1 - 1 / (1 + r / 12) ^ n)
      q_time <- if (version == 1) generate_q_time(r, n) else (1:n) / n
      (sum(vapply(q_time, S, numeric(1)) / (1 + b / 12) ^ (1:n)) - 1 / I) ^ 2
    }
    return(tryCatch(optimise(fcn2optimise, c(-1, 29))$minimum, error = function(...) NA))
  } else if (version == 3) {
    if (!is.atomic(S) && !is.vector(S)) return(NA)
    if (length(S) != n) return(NA)
    fcn2optimise <- function(r) {
      I <- if (abs(r) <= 10 * .Machine$double.eps) 1 / n
           else r / 12 / (1 - 1 / (1 + r / 12) ^ n)
      (sum(S / (1 + b / 12) ^ (1:n)) - 1 / I) ^ 2
    }
    return(tryCatch(optimise(fcn2optimise, c(-1, 29))$minimum, error = function(...) NA))
  }
  NA
}
