/*================================================================================

File: OrderBook.cpp                                                             
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-03-07 21:17:51                                                 
last edited: 2025-05-10 22:09:22                                                

================================================================================*/

#pragma once

#include <ranges>

#include "OrderBook.hpp"
#include "macros.hpp"

template<typename PriceType, typename QtyType>
COLD OrderBook<PriceType, QtyType>::OrderBook(void) noexcept
{
  bids.prices.push_back(INT32_MIN);
  asks.prices.push_back(INT32_MAX);
  bids.cumulative_qtys.push_back(0);
  asks.cumulative_qtys.push_back(0);
}

template<typename PriceType, typename QtyType>
COLD OrderBook<PriceType, QtyType>::OrderBook(OrderBook &&other) noexcept :
  bids(std::move(other.bids)),
  asks(std::move(other.asks))
{
}

template<typename PriceType, typename QtyType>
COLD OrderBook<PriceType, QtyType>::~OrderBook()
{
}

//TODO possible to make branchless by inhibition
template<typename PriceType, typename QtyType>
template<typename Comparator>
HOT void OrderBook<PriceType, QtyType>::setQty(PriceLevels &levels, const PriceType price, const QtyType qty)
{
  auto &prices = levels.prices;
  auto &cumulative_qtys = levels.cumulative_qtys;

  //TODO backwards linear search would be faster (updates likely to be at the end). (if you hire me, i will implement a SIMD one :D )
  static constexpr Comparator cmp;
  const auto price_it = std::lower_bound(prices.begin(), prices.end(), price, cmp);
  const auto qty_it = cumulative_qtys.begin() + std::distance(prices.begin(), price_it);

  if (qty == 0) [[unlikely]]
  {
    prices.erase(price_it);
    cumulative_qtys.erase(qty_it);
    return;
  }
  else if (*price_it != price) [[unlikely]]
  {
    prices.insert(price_it, price);
    cumulative_qtys.insert(qty_it, qty);
    return;
  }

  *qty_it += qty;
}