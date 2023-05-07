
#pragma once
#include <cmath>
#include <limits>

namespace Approx {
template <typename TReal>
static bool
isApproxEqual(TReal a, TReal b,
              TReal tolerance = std::numeric_limits<TReal>::epsilon()) {
  TReal diff = std::fabs(a - b);
  //   std::cout << "diff: " << diff << std::endl;
  if (diff <= tolerance) {
    // std::cout << "true: " << diff << std::endl;
    return true;
  }

  return false;
}

// supply tolerance that is meaningful in your context
// for example, default tolerance may not work if you are comparing
// double with float
template <typename TReal>
static bool
isApporxZero(TReal a, TReal tolerance = std::numeric_limits<TReal>::epsilon()) {
  if (std::fabs(a) <= tolerance)
    return true;
  return false;
}

// use this when you want to be on safe side
// for example, don't start rover unless signal is above 1
template <typename TReal>
static bool
isDefLessThan(TReal a, TReal b,
              TReal tolerance = std::numeric_limits<TReal>::epsilon()) {
  TReal diff = a - b;
  if (diff > 0)
    return false;
  if (std::fabs(diff) > tolerance)
    return true;

  return false;
}
template <typename TReal>
static bool
isDefGreaterThan(TReal a, TReal b,
                 TReal tolerance = std::numeric_limits<TReal>::epsilon()) {
  TReal diff = a - b;
  if (diff < 0)
    return false;
  if (std::fabs(diff) > tolerance)
    return true;

  return false;
}

// implements ULP method
// use this when you are only concerned about floating point precision
// issue for example, if you want to see if a is 1.0 by checking if its
// within 10 closest representable floating point numbers around 1.0.
template <typename TReal>
static bool isWithinPrecisionInterval(TReal a, TReal b,
                                      unsigned int interval_size = 1) {
  TReal min_a =
      a - (a - std::nextafter(a, std::numeric_limits<TReal>::lowest())) *
              interval_size;
  TReal max_a = a + (std::nextafter(a, std::numeric_limits<TReal>::max()) - a) *
                        interval_size;

  return min_a <= b && max_a >= b;
}
} // namespace Approx
