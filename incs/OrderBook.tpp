/*================================================================================

File: OrderBook.tpp                                                             
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-05-10 11:37:51                                                 
last edited: 2025-05-10 11:37:51                                                

================================================================================*/

#pragma once

#include "OrderBook.hpp"
#include "macros.hpp"

//TODO possible to make branchless by inhibition
template<typename PriceType, typename QtyType>
template<typename Comparator>
HOT void OrderBook<PriceType, QtyType>::setQty(PriceLevels &levels, const PriceType price, const QtyType qty)
{
  auto &prices = levels.prices;
  auto &cumulative_qtys = levels.cumulative_qtys;

  //TODO backwards linear search would be faster (updates likely to be at the end). (if you hire me, i will implement a SIMD one :D )
  static constexpr Comparator cmp;
  const auto price_it = prices.lower_bound(price, cmp);
  const auto qty_it = cumulative_qtys.begin() + std::distance(prices.begin(), price_it);

  if (qty == 0) [[unlikely]]
  {
    prices.erase(price_it);
    cumulative_qtys.erase(qty_it);
    return;
  }
  else if (price_it->get() != price) [[unlikely]]
  {
    prices.insert(price_it, price);
    cumulative_qtys.insert(qty_it, qty);
    return;
  }

  qty_it->set(qty);
}