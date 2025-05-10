/*================================================================================

File: OrderBook.hpp                                                             
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-03-22 14:14:57                                                 
last edited: 2025-05-10 09:38:35                                                

================================================================================*/

#pragma once

#include <cstdint>
#include <array>
#include <vector>

#include "macros.hpp"

class OrderBook
{
  public:
    OrderBook(void) noexcept;
    OrderBook(OrderBook &&) noexcept;
    ~OrderBook();

    enum Side : uint8_t { BID = 0, ASK = 1 };

    inline int32_t getBestBidPrice(void) const noexcept;
    inline int32_t getBestAskPrice(void) const noexcept;
    inline uint64_t getBestBidQty(void) const noexcept;
    inline uint64_t getBestAskQty(void) const noexcept;

    void addOrder(const Side side, const int32_t price, const uint64_t qty);
    void removeOrder(const Side side, const int32_t price, const uint64_t qty);
    void executeOrder(const Side side, const uint64_t qty);

  private:

    struct PriceLevels
    {
      //sorted with best price last. prices[i] and cumulative_qtys[i] are the same price level
      std::vector<int32_t> prices;
      std::vector<uint64_t> cumulative_qtys;
    };

    std::array<PriceLevels, 2> book_sides;

    inline void addOrderBid(const int32_t price, const uint64_t qty);
    inline void addOrderAsk(const int32_t price, const uint64_t qty);
    template<typename Comparator>
    void addOrder(PriceLevels &levels, const int32_t price, const uint64_t qty);
    void addPriceLevel(PriceLevels &levels, const size_t price_idx, const int32_t price);

    inline void removeOrderBid(const int32_t price, const uint64_t qty);
    inline void removeOrderAsk(const int32_t price, const uint64_t qty);
    template<typename Comparator>
    void removeOrder(PriceLevels &levels, const int32_t price, const uint64_t qty);
    void removePriceLevel(PriceLevels &levels, const size_t price_idx);
};

#include "OrderBook.inl"