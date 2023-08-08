#include "Float.hxx"

Float_Binary_Precision::Float_Binary_Precision(unsigned int binary_precision)
    : old_decimal_precision(Float::default_precision())
{
  unsigned int decimal_precision
    = std::ceil(binary_precision * std::log10(2.0)) + 1;
  Float::default_precision(decimal_precision);
}
Float_Binary_Precision::~Float_Binary_Precision()
{
  Float::default_precision(old_decimal_precision);
}
