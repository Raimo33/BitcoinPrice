/*================================================================================

File: OrderBook.cpp                                                             
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-03-07 21:17:51                                                 
last edited: 2025-05-11 21:29:03                                                

================================================================================*/

#pragma once

#include <ranges>

#include "OrderBook.hpp"
#include "macros.hpp"

template<typename PriceType, typename QtyType>
COLD OrderBook<PriceType, QtyType>::OrderBook(void) noexcept
{
  //TODO initialize bids with MIN and asks with MAX for better safety (now possible segfault if new order at negative price)
  _bids.prices.push_back(0); //std::numeric_limits<PriceType>::min());
  _asks.prices.push_back(0); //std::numeric_limits<PriceType>::max());
  _bids.cumulative_qtys.push_back(0);
  _asks.cumulative_qtys.push_back(0);
}

template<typename PriceType, typename QtyType>
COLD OrderBook<PriceType, QtyType>::OrderBook(OrderBook &&other) noexcept :
  _bids(std::move(other._bids)),
  _asks(std::move(other._asks))
{
}

template<typename PriceType, typename QtyType>
COLD OrderBook<PriceType, QtyType>::~OrderBook()
{
}

template<typename PriceType, typename QtyType>
template<typename Comparator>
HOT void OrderBook<PriceType, QtyType>::setQty(PriceLevels &levels, const PriceType price, const QtyType qty)
{
  auto &prices = levels.prices;
  auto &cumulative_qtys = levels.cumulative_qtys;

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