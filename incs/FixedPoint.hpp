/*================================================================================

File: FixedPoint.hpp                                                            
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-05-10 11:37:51                                                 
last edited: 2025-05-10 22:09:22                                                

================================================================================*/

#pragma once

#include <cstdint>
#include <cmath>
#include <ostream>

template <uint8_t Decimals>
struct FixedPoint
{
  int64_t value;
  static constexpr int64_t scale = std::pow(10, Decimals);

  FixedPoint();
  FixedPoint(const int64_t integer);
  explicit FixedPoint(const double real);
  explicit FixedPoint(const int64_t int_part, const int64_t frac_part);
  explicit FixedPoint(std::string_view str);
  FixedPoint(const FixedPoint &other);
  FixedPoint(FixedPoint &&other) noexcept;

  FixedPoint operator+(const FixedPoint &other) const;
  FixedPoint operator-(const FixedPoint &other) const;
  FixedPoint operator*(const FixedPoint &other) const;
  FixedPoint operator/(const FixedPoint &other) const;
  bool operator==(const FixedPoint &other) const;
  bool operator!=(const FixedPoint &other) const;
  bool operator<(const FixedPoint &other) const;
  bool operator<=(const FixedPoint &other) const;
  bool operator>(const FixedPoint &other) const;
  bool operator>=(const FixedPoint &other) const;
  FixedPoint &operator+=(const FixedPoint &other);
  FixedPoint &operator-=(const FixedPoint &other);
  FixedPoint &operator*=(const FixedPoint &other);
  FixedPoint &operator/=(const FixedPoint &other);
  FixedPoint &operator=(const FixedPoint &other);
  FixedPoint &operator=(FixedPoint &&other) noexcept;
};

template <uint8_t Decimals>
std::ostream &operator<<(std::ostream &os, const FixedPoint<Decimals> &fp);

#include "FixedPoint.tpp"