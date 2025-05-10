/*================================================================================

File: OrderBook.cpp                                                             
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-03-07 21:17:51                                                 
last edited: 2025-05-10 11:37:51                                                

================================================================================*/

#include <ranges>

#include "OrderBook.hpp"
#include "macros.hpp"

template<typename PriceType, typename QtyType>
COLD OrderBook<PriceType, QtyType>::OrderBook(void) noexcept :
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