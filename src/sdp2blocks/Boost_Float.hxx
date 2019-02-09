#pragma once

#include <boost/multiprecision/mpfr.hpp>
#include <sstream>

using Boost_Float = boost::multiprecision::mpfr_float;

inline std::string to_string(const Boost_Float &boost_float)
{
  // Using a stringstream seems to the best way to convert between
  // MPFR and GMP.  It may lose a bit or two since string
  // conversion is not sufficient for round-tripping.
  std::stringstream ss;
  ss.precision(Boost_Float::default_precision());
  ss << boost_float;
  return ss.str();
}

