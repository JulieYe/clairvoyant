#include <Rcpp.h>
using namespace Rcpp;

//' Numeric limit of a min float
//'
//' @export
// [[Rcpp::export]]
float numeric_limits_min_float() {
  return std::numeric_limits<float>::min();
}

//' Numeric limit of a max float
//'
//' @export
// [[Rcpp::export]]
float numeric_limits_max_float() {
  return std::numeric_limits<float>::max();
}
