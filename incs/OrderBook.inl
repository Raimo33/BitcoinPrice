/*================================================================================

File: OrderBook.inl                                                             
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-05-10 11:37:51                                                 
last edited: 2025-05-10 11:37:51                                                

================================================================================*/

#pragma once

#include "OrderBook.hpp"
#include "macros.hpp"

template<typename PriceType, typename QtyType>
HOT ALWAYS_INLINE inline PriceType OrderBook<PriceType, QtyType>::getBestBidPrice(void) const noexcept
{
  return _bids.prices.back();
}

template<typename PriceType, typename QtyType>
HOT ALWAYS_INLINE inline PriceType OrderBook<PriceType, QtyType>::getBestAskPrice(void) const noexcept
{
  return _asks.prices.back();
}

template<typename PriceType, typename QtyType>
HOT ALWAYS_INLINE inline QtyType OrderBook<PriceType, QtyType>::getBestBidQty(void) const noexcept
{
  return _bids.cumulative_qtys.back();
}

template<typename PriceType, typename QtyType>
HOT ALWAYS_INLINE inline QtyType OrderBook<PriceType, QtyType>::getBestAskQty(void) const noexcept
{
  return _asks.cumulative_qtys.back();
}

template<typename PriceType, typename QtyType>
HOT ALWAYS_INLINE inline void OrderBook<PriceType, QtyType>::setBidQty(const PriceType price, const QtyType qty)
{
  setQty<std::less_equal<PriceType>>(_bids, price, qty);
}

template<typename PriceType, typename QtyType>
HOT ALWAYS_INLINE inline void OrderBook<PriceType, QtyType>::setAskQty(const PriceType price, const QtyType qty)
{
  setQty<std::greater_equal<PriceType>>(_asks, price, qty);
}
