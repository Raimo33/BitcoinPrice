#pragma once

#include "macros.hpp"

namespace messages
{

#pragma pack(push, 1)

//TODO use fixed point numbers for faster construction/printing/operations. But since gemini API uses floats, I will use floats too.
struct alignas(CACHELINE_SIZE) TopOfBook
{
  float best_bid_price;
  float best_ask_price;
  float best_bid_qty;
  float best_ask_qty;

  bool operator==(const TopOfBook &other) const noexcept {
    return (best_bid_price == other.best_bid_price) &
           (best_ask_price == other.best_ask_price) &
           (best_bid_qty == other.best_bid_qty) &
           (best_ask_qty == other.best_ask_qty);
  }
};

#pragma pack(pop)

} // namespace messages