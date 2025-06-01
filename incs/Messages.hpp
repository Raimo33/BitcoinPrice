#pragma once

#include "macros.hpp"

namespace messages
{

#pragma pack(push, 1)

template <typename PriceType, typename QtyType>
struct alignas(CACHELINE_SIZE) TopOfBook
{
  PriceType best_bid_price;
  PriceType best_ask_price;
  QtyType best_bid_qty;
  QtyType best_ask_qty;
};

#pragma pack(pop)

} // namespace messages