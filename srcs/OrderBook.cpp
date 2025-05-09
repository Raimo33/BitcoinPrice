/*================================================================================

File: OrderBook.cpp                                                             
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-03-07 21:17:51                                                 
last edited: 2025-05-09 22:25:43                                                

================================================================================*/

#include <ranges>

#include "OrderBook.hpp"
#include "utils/utils.hpp"
#include "macros.hpp"

COLD OrderBook::OrderBook(void) noexcept
{
  PriceLevels &bids = book_sides[BID];
  PriceLevels &asks = book_sides[ASK];

  bids.prices.push_back(INT32_MIN);
  asks.prices.push_back(INT32_MAX);
  bids.cumulative_qtys.push_back(0);
  asks.cumulative_qtys.push_back(0);
}

COLD OrderBook::OrderBook(OrderBook &&other) noexcept :
  book_sides{std::move(other.book_sides)}
{
}

COLD OrderBook::~OrderBook()
{
}

HOT void OrderBook::addOrder(const Side side, const int32_t price, const uint64_t qty)
{
  using Handler = void (OrderBook::*)(const int32_t, const uint64_t);
  static constexpr Handler handlers[] = {
    &OrderBook::addOrderBid,
    &OrderBook::addOrderAsk
  };

  (this->*handlers[side])(price, qty);
}

HOT void OrderBook::removeOrder(const Side side, const int32_t price, const uint64_t qty)
{
  using Handler = void (OrderBook::*)(const int32_t, const uint64_t);
  static constexpr Handler handlers[] = {
    &OrderBook::removeOrderBid,
    &OrderBook::removeOrderAsk
  };

  (this->*handlers[side])(price, qty);
}

//TODO used???
HOT void OrderBook::executeOrder(const Side side, const uint64_t qty)
{
  PriceLevels &levels = book_sides[side];
  const size_t price_idx = levels.prices.size() - 1;

  auto &cumulative_qty = levels.cumulative_qtys.back();
  cumulative_qty -= qty;

  if (cumulative_qty == 0) [[unlikely]]
    removePriceLevel(levels, price_idx);
}

template<typename Comparator>
HOT void OrderBook::addOrder(PriceLevels &levels, const int32_t price, const uint64_t qty)
{
  const auto &prices = levels.prices;

  static constexpr Comparator cmp;
  size_t price_idx = utils::backward_lower_bound(std::span<const int32_t>{prices}, price, cmp);

  if (prices[price_idx] != price) [[unlikely]]
    addPriceLevel(levels, ++price_idx);

  auto &cumulative_qty = levels.cumulative_qtys[price_idx];
  cumulative_qty += qty;
}

HOT void OrderBook::addPriceLevel(PriceLevels &levels, const size_t price_idx)
{
  auto &prices = levels.prices;

  prices.insert(prices.cbegin() + price_idx, 0);
}

template<typename Comparator>
HOT void OrderBook::removeOrder(PriceLevels &levels, const int32_t price, const uint64_t qty)
{
  static constexpr Comparator prices_cmp;
  const size_t price_idx = utils::backward_lower_bound(std::span<const int32_t>{levels.prices}, price, prices_cmp);

  auto &cumulative_qty = levels.cumulative_qtys[price_idx];
  cumulative_qty -= qty;

  if (cumulative_qty == 0) [[unlikely]]
    removePriceLevel(levels, price_idx);
}

HOT void OrderBook::removePriceLevel(PriceLevels &levels, const size_t price_idx)
{
  auto &prices = levels.prices;
  auto &cumulative_qtys = levels.cumulative_qtys;

  prices.erase(prices.cbegin() + price_idx);
  cumulative_qtys.erase(cumulative_qtys.cbegin() + price_idx);
}