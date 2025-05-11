/*================================================================================

File: FixedPoint.tpp                                                            
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-05-10 22:09:22                                                 
last edited: 2025-05-10 22:09:22                                                

================================================================================*/

#pragma once

#include "FixedPoint.hpp"

template <uint8_t Decimals>
FixedPoint<Decimals>::FixedPoint()
 : value(0) {}

template <uint8_t Decimals>
FixedPoint<Decimals>::FixedPoint(const int64_t integer)
 : value(integer * scale) {}

template <uint8_t Decimals>
FixedPoint<Decimals>::FixedPoint(const double real)
 : value(static_cast<int64_t>(real * scale)) {}

template <uint8_t Decimals>
FixedPoint<Decimals>::FixedPoint(const int64_t int_part, const int64_t frac_part)
 : value(int_part * scale + frac_part) {}

template <uint8_t Decimals>
FixedPoint<Decimals>::FixedPoint(std::string_view str)
 : value(0)
{
  const char *str_ptr = str.data();
  int64_t int_part = std::strtoll(str_ptr, const_cast<char **>(&str_ptr), 10);
  
  int64_t frac_part = 0;
  if (str_ptr[0] == '.')
    frac_part = std::strtoll(str_ptr + 1, nullptr, 10);

  value = int_part * scale + frac_part;
}

template <uint8_t Decimals>
FixedPoint<Decimals>::FixedPoint(const FixedPoint &other)
 : value(other.value) {}

template <uint8_t Decimals>
FixedPoint<Decimals>::FixedPoint(FixedPoint &&other) noexcept
 : value(other.value)
{
  other.value = 0;
}

template <uint8_t Decimals>
FixedPoint<Decimals> FixedPoint<Decimals>::operator+(const FixedPoint &other) const
{
  return FixedPoint<Decimals>(value + other.value);
}

template <uint8_t Decimals>
FixedPoint<Decimals> FixedPoint<Decimals>::operator-(const FixedPoint &other) const
{
  return FixedPoint<Decimals>(value - other.value);
}

template <uint8_t Decimals>
FixedPoint<Decimals> FixedPoint<Decimals>::operator*(const FixedPoint &other) const
{
  return FixedPoint<Decimals>((value * other.value) / scale);
}

template <uint8_t Decimals>
FixedPoint<Decimals> FixedPoint<Decimals>::operator/(const FixedPoint &other) const
{
  return FixedPoint<Decimals>((value * scale) / other.value);
}

template <uint8_t Decimals>
bool FixedPoint<Decimals>::operator==(const FixedPoint &other) const
{
  return value == other.value;
}

template <uint8_t Decimals>
bool FixedPoint<Decimals>::operator!=(const FixedPoint &other) const
{
  return value != other.value;
}

template <uint8_t Decimals>
bool FixedPoint<Decimals>::operator<(const FixedPoint &other) const
{
  return value < other.value;
}

template <uint8_t Decimals>
bool FixedPoint<Decimals>::operator<=(const FixedPoint &other) const
{
  return value <= other.value;
}

template <uint8_t Decimals>
bool FixedPoint<Decimals>::operator>(const FixedPoint &other) const
{
  return value > other.value;
}

template <uint8_t Decimals>
bool FixedPoint<Decimals>::operator>=(const FixedPoint &other) const
{
  return value >= other.value;
}

template <uint8_t Decimals>
FixedPoint<Decimals> &FixedPoint<Decimals>::operator+=(const FixedPoint &other)
{
  value += other.value;
  return *this;
}

template <uint8_t Decimals>
FixedPoint<Decimals> &FixedPoint<Decimals>::operator-=(const FixedPoint &other)
{
  value -= other.value;
  return *this;
}

template <uint8_t Decimals>
FixedPoint<Decimals> &FixedPoint<Decimals>::operator*=(const FixedPoint &other)
{
  value = (value * other.value) / scale;
  return *this;
}

template <uint8_t Decimals>
FixedPoint<Decimals> &FixedPoint<Decimals>::operator/=(const FixedPoint &other)
{
  value = (value * scale) / other.value;
  return *this;
}

template <uint8_t Decimals>
FixedPoint<Decimals> &FixedPoint<Decimals>::operator=(const FixedPoint &other)
{
  if (this != &other)
    value = other.value;

  return *this;
}

template <uint8_t Decimals>
FixedPoint<Decimals> &FixedPoint<Decimals>::operator=(FixedPoint &&other) noexcept
{
  if (this != &other)
  {
    value = other.value;
    other.value = 0;
  }

  return *this;
}

template <uint8_t Decimals>
std::ostream &operator<<(std::ostream &os, const FixedPoint<Decimals> &fp)
{
  os << static_cast<double>(fp.value) / FixedPoint<Decimals>::scale;
  return os;
}