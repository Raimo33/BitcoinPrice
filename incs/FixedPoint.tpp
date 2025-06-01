/*================================================================================

File: FixedPoint.tpp                                                            
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-05-10 22:09:22                                                 
last edited: 2025-05-13 16:55:56                                                

================================================================================*/

#pragma once

#include <algorithm>

#include "FixedPoint.hpp"
#include "macros.hpp"

template <uint8_t Decimals>
FixedPoint<Decimals>::FixedPoint() noexcept
 : value(0) {}

template <uint8_t Decimals>
FixedPoint<Decimals>::FixedPoint(const int32_t integer) noexcept
 : value(integer * scale) {}

template <uint8_t Decimals>
FixedPoint<Decimals>::FixedPoint(const double real) noexcept
 : value(static_cast<int32_t>(real * scale)) {}

template <uint8_t Decimals>
FixedPoint<Decimals>::FixedPoint(const int32_t int_part, const int32_t frac_part) noexcept
 : value(int_part * scale + frac_part) {}

template <uint8_t Decimals>
FixedPoint<Decimals>::FixedPoint(std::string_view str) noexcept
 : value(0)
{
  const char *restrict str_ptr = str.data();
  const int32_t int_part = std::strtoll(str_ptr, const_cast<char **>(&str_ptr), 10);

  int32_t frac_part = 0;
  if (str_ptr[0] == '.') [[likely]]
    frac_part = std::strtoll(str_ptr + 1, nullptr, 10);

  value = int_part * scale + frac_part;
}

template <uint8_t Decimals>
FixedPoint<Decimals>::FixedPoint(const FixedPoint &other) noexcept
 : value(other.value) {}

template <uint8_t Decimals>
FixedPoint<Decimals>::FixedPoint(FixedPoint &&other) noexcept
 : value(other.value)
{
  other.value = 0;
}

template <uint8_t Decimals>
FixedPoint<Decimals> FixedPoint<Decimals>::operator+(const FixedPoint &other) const noexcept
{
  return FixedPoint<Decimals>(value + other.value);
}

template <uint8_t Decimals>
FixedPoint<Decimals> FixedPoint<Decimals>::operator-(const FixedPoint &other) const noexcept
{
  return FixedPoint<Decimals>(value - other.value);
}

template <uint8_t Decimals>
FixedPoint<Decimals> FixedPoint<Decimals>::operator-() const noexcept
{
  return FixedPoint<Decimals>(-value);
}

template <uint8_t Decimals>
FixedPoint<Decimals> FixedPoint<Decimals>::operator*(const FixedPoint &other) const noexcept
{
  return FixedPoint<Decimals>((value * other.value) / scale);
}

template <uint8_t Decimals>
FixedPoint<Decimals> FixedPoint<Decimals>::operator/(const FixedPoint &other) const noexcept
{
  return FixedPoint<Decimals>((value * scale) / other.value);
}

template <uint8_t Decimals>
bool FixedPoint<Decimals>::operator==(const FixedPoint &other) const noexcept
{
  return value == other.value;
}

template <uint8_t Decimals>
bool FixedPoint<Decimals>::operator!=(const FixedPoint &other) const noexcept
{
  return value != other.value;
}

template <uint8_t Decimals>
bool FixedPoint<Decimals>::operator<(const FixedPoint &other) const noexcept
{
  return value < other.value;
}

template <uint8_t Decimals>
bool FixedPoint<Decimals>::operator<=(const FixedPoint &other) const noexcept
{
  return value <= other.value;
}

template <uint8_t Decimals>
bool FixedPoint<Decimals>::operator>(const FixedPoint &other) const noexcept
{
  return value > other.value;
}

template <uint8_t Decimals>
bool FixedPoint<Decimals>::operator>=(const FixedPoint &other) const noexcept
{
  return value >= other.value;
}

template <uint8_t Decimals>
FixedPoint<Decimals> &FixedPoint<Decimals>::operator+=(const FixedPoint &other) noexcept
{
  value += other.value;
  return *this;
}

template <uint8_t Decimals>
FixedPoint<Decimals> &FixedPoint<Decimals>::operator-=(const FixedPoint &other) noexcept
{
  value -= other.value;
  return *this;
}

template <uint8_t Decimals>
FixedPoint<Decimals> &FixedPoint<Decimals>::operator*=(const FixedPoint &other) noexcept
{
  value = (value * other.value) / scale;
  return *this;
}

template <uint8_t Decimals>
FixedPoint<Decimals> &FixedPoint<Decimals>::operator/=(const FixedPoint &other) noexcept
{
  value = (value * scale) / other.value;
  return *this;
}

template <uint8_t Decimals>
FixedPoint<Decimals> &FixedPoint<Decimals>::operator=(const FixedPoint &other) noexcept
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

//TODO can be optimized further (lookup tables, simd, division and modulus in a single step, etc.)
//TODO fix subtle bug when printing zero
template <uint8_t Decimals>
void FixedPoint<Decimals>::format(char *restrict buffer, FixedPoint<Decimals> fixed_point) noexcept
{
  const bool is_negative = fixed_point < 0;

  if (is_negative)
  {
    *buffer++ = '-';
    fixed_point = -fixed_point;
  }

  int32_t integer_part = fixed_point.value / scale;
  int32_t fractional_part = fixed_point.value % scale;

  char *ptr = buffer;
  do {
    *ptr++ = '0' + (integer_part % 10);
    integer_part /= 10;
  } while (integer_part > 0);

  std::reverse(buffer, ptr);

  *ptr++ = '.';

  for (uint8_t i = 0; i < Decimals; ++i)
  {
    fractional_part *= 10;
    *ptr++ = '0' + (fractional_part / scale);
    fractional_part %= scale;
  }
}
