/*================================================================================

File: OrderBook.hpp                                                             
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-03-22 14:14:57                                                 
last edited: 2025-05-11 21:29:03                                                

================================================================================*/

#pragma once

#include <cstdint>
#include <array>
#include <vector>

#include "macros.hpp"

template <typename PriceType, typename QtyType>
class OrderBook
{
  public:
    OrderBook(void) noexcept;
    OrderBook(OrderBook &&) noexcept;
    ~OrderBook();

    inline PriceType getBestBidPrice(void) const noexcept;
    inline PriceType getBestAskPrice(void) const noexcept;
    inline QtyType getBestBidQty(void) const noexcept;
    inline QtyType getBestAskQty(void) const noexcept;

    inline void setBidQty(const PriceType price, const QtyType qty);
    inline void setAskQty(const PriceType price, const QtyType qty);

  private:

    struct PriceLevels
    {
      std::vector<PriceType> prices;
      std::vector<QtyType> cumulative_qtys;
    };

    PriceLevels bids;
    PriceLevels asks;

    template<typename Comparator>
    void setQty(PriceLevels &levels, const PriceType price, const QtyType qty);
};

#include "OrderBook.inl"
#include "OrderBook.tpp"