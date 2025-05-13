/*================================================================================

File: Logger.tpp                                                                
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-05-13 14:30:09                                                 
last edited: 2025-05-13 14:30:09                                                

================================================================================*/

#pragma once

#include "Logger.hpp"

template <uint8_t PriceDecimals, uint8_t QtyDecimals>
Logger<PriceDecimals, QtyDecimals>::Logger(std::string_view pair) noexcept :
  pair(pair),
  queue(this->pair + "_top_of_book") {}

template <uint8_t PriceDecimals, uint8_t QtyDecimals>
Logger<PriceDecimals, QtyDecimals>::~Logger(void) noexcept
{
}

template <uint8_t PriceDecimals, uint8_t QtyDecimals>
void Logger<PriceDecimals, QtyDecimals>::start(void)
{
  TopOfBook cached_top_of_book{};

  while (true)
  {
    if (queue.isEmpty())
      continue;

    TopOfBook top_of_book = queue.pop();
    if (top_of_book == cached_top_of_book) [[unlikely]]
      continue;

    cached_top_of_book = std::move(top_of_book);
    log(top_of_book);
  }
}

template <uint8_t PriceDecimals, uint8_t QtyDecimals>
void Logger<PriceDecimals, QtyDecimals>::log(const TopOfBook& top_of_book)
{
  std::cout << top_of_book.best_bid_price << " " << top_of_book.best_bid_qty << " - "
            << top_of_book.best_ask_price << " " << top_of_book.best_ask_qty << std::endl;
}

template <uint8_t PriceDecimals, uint8_t QtyDecimals>
bool Logger<PriceDecimals, QtyDecimals>::TopOfBook::operator==(const TopOfBook& other) const noexcept
{
  bool equal = true;

  equal &= best_bid_price == other.best_bid_price;
  equal &= best_ask_price == other.best_ask_price;
  equal &= best_bid_qty == other.best_bid_qty;
  equal &= best_ask_qty == other.best_ask_qty;

  return equal;
}
