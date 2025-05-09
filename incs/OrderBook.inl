/*================================================================================

File: OrderBook.inl                                                             
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-03-23 17:58:46                                                 
last edited: 2025-05-09 22:25:43                                                

================================================================================*/

#pragma once

#include "OrderBook.hpp"
#include "macros.hpp"

HOT ALWAYS_INLINE inline int32_t OrderBook::getBestBidPrice(void) const noexcept
{
  return book_sides[BID].prices.back();
}

HOT ALWAYS_INLINE inline int32_t OrderBook::getBestAskPrice(void) const noexcept
{
  return book_sides[ASK].prices.back();
}

HOT ALWAYS_INLINE inline uint64_t OrderBook::getBestBidQty(void) const noexcept
{
  return book_sides[BID].cumulative_qtys.back();
}

HOT ALWAYS_INLINE inline uint64_t OrderBook::getBestAskQty(void) const noexcept
{
  return book_sides[ASK].cumulative_qtys.back();
}

HOT ALWAYS_INLINE inline void OrderBook::addOrderBid(const int32_t price, const uint64_t qty)
{
  addOrder<std::less_equal<int32_t>>(book_sides[BID], price, qty);
}

HOT ALWAYS_INLINE inline void OrderBook::addOrderAsk(const int32_t price, const uint64_t qty)
{
  addOrder<std::greater_equal<int32_t>>(book_sides[ASK], price, qty);
}

HOT ALWAYS_INLINE inline void OrderBook::removeOrderBid(const int32_t price, const uint64_t qty)
{
  removeOrder<std::less_equal<int32_t>>(book_sides[BID], price, qty);
}

HOT ALWAYS_INLINE inline void OrderBook::removeOrderAsk(const int32_t price, const uint64_t qty)
{
  removeOrder<std::greater_equal<int32_t>>(book_sides[ASK], price, qty);
}