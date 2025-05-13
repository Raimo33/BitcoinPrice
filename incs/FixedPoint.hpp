/*================================================================================

File: FixedPoint.hpp                                                            
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-05-10 11:37:51                                                 
last edited: 2025-05-13 16:40:17                                                

================================================================================*/

#pragma once

#include <cstdint>
#include <cmath>
#include <ostream>

template <uint8_t Decimals>
struct FixedPoint
{
  int32_t value;
  static constexpr int32_t scale = std::pow(10, Decimals);

  FixedPoint();
  FixedPoint(const int32_t integer);
  explicit FixedPoint(const double real);
  explicit FixedPoint(const int32_t int_part, const int32_t frac_part);
  explicit FixedPoint(std::string_view str);
  FixedPoint(const FixedPoint& other);
  FixedPoint(FixedPoint&& other) noexcept;

  FixedPoint operator+(const FixedPoint& other) const;
  FixedPoint operator-(const FixedPoint& other) const;
  FixedPoint operator-() const;
  FixedPoint operator*(const FixedPoint& other) const;
  FixedPoint operator/(const FixedPoint& other) const;
  bool operator==(const FixedPoint& other) const;
  bool operator!=(const FixedPoint& other) const;
  bool operator<(const FixedPoint& other) const;
  bool operator<=(const FixedPoint& other) const;
  bool operator>(const FixedPoint& other) const;
  bool operator>=(const FixedPoint& other) const;
  FixedPoint& operator+=(const FixedPoint& other);
  FixedPoint& operator-=(const FixedPoint& other);
  FixedPoint& operator*=(const FixedPoint& other);
  FixedPoint& operator/=(const FixedPoint& other);
  FixedPoint& operator=(const FixedPoint& other);
  FixedPoint& operator=(FixedPoint&& other) noexcept;

  static void format(char* buffer, FixedPoint fixed_point);
};

#include "FixedPoint.tpp"