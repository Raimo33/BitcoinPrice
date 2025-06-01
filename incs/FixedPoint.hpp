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

#include "macros.hpp"

template <uint8_t Decimals>
struct FixedPoint
{
  int32_t value;
  static constexpr int32_t scale = std::pow(10, Decimals);

  FixedPoint() noexcept;
  FixedPoint(const int32_t integer) noexcept;
  explicit FixedPoint(const double real) noexcept;
  explicit FixedPoint(const int32_t int_part, const int32_t frac_part) noexcept;
  explicit FixedPoint(std::string_view str) noexcept;
  FixedPoint(const FixedPoint &other) noexcept;
  FixedPoint(FixedPoint &&other) noexcept;

  FixedPoint operator+(const FixedPoint &other) const noexcept;
  FixedPoint operator-(const FixedPoint &other) const noexcept;
  FixedPoint operator-() const noexcept;
  FixedPoint operator*(const FixedPoint &other) const noexcept;
  FixedPoint operator/(const FixedPoint &other) const noexcept;
  bool operator==(const FixedPoint &other) const noexcept;
  bool operator!=(const FixedPoint &other) const noexcept;
  bool operator<(const FixedPoint &other) const noexcept;
  bool operator<=(const FixedPoint &other) const noexcept;
  bool operator>(const FixedPoint &other) const noexcept;
  bool operator>=(const FixedPoint &other) const noexcept;
  FixedPoint& operator+=(const FixedPoint &other) noexcept;
  FixedPoint& operator-=(const FixedPoint &other) noexcept;
  FixedPoint& operator*=(const FixedPoint &other) noexcept;
  FixedPoint& operator/=(const FixedPoint &other) noexcept;
  FixedPoint& operator=(const FixedPoint &other) noexcept;
  FixedPoint& operator=(FixedPoint &&other) noexcept;

  static void format(char *restrict buffer, FixedPoint fixed_point) noexcept;
};

#include "FixedPoint.tpp"