context("amortization")

test_that("Last one should be zero", {
  expect_equal(p_to_q(12, 0.05, 12), 1)
})

test_that("Calibrate APR for q time", {
  s0 <- function(x) if (x <= 0.5) 1 else 0.5
  s1 <- function(x) if (x <= 0.5) 1 else 0.4
  expect_true(calibrate_apr(s0, 0.1, 12, 1) < 
    calibrate_apr(s1, 0.1, 12, 1))
})

test_that("Calibrate APR for p and q times", {
  s <- function(x) if (x <= 0.5) 1 else 0.5
  expect_true(calibrate_apr(s, 0.1, 12, 1) < 
    calibrate_apr(s, 0.1, 12, 2))
})

test_that("Calibrate APR on installments", {
  s0 <- seq(1, 1/12, length.out = 12)
  s1 <- seq(1, 1/24, length.out = 12)
  expect_true(calibrate_apr(s0, 0.1, 12, 3) < 
    calibrate_apr(s1, 0.1, 12, 3))
})
