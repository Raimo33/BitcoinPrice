/*================================================================================

File: Logger.tpp                                                                
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-05-13 14:30:09                                                 
last edited: 2025-05-13 16:40:17                                                

================================================================================*/

#include <format>

#include "Logger.hpp"
#include "utils.hpp"

Logger::Logger(std::string_view pair) noexcept :
  _pair(pair),
  _shared_fd(utils::get_shared_memory_fd(_pair)),
  _queue(_shared_fd) {}

Logger::~Logger(void) noexcept
{
  close(_shared_fd);
}

void Logger::start(void)
{
  TopOfBook top_of_book;

  while (true)
  {
    while (!_queue.pop(top_of_book))
      ;
    try_log(top_of_book);
  }

  //TODO std::unreachable() in c++23
}

void Logger::try_log(const TopOfBook &top_of_book)
{
  static thread_local float cached_bid_price;
  static thread_local float cached_ask_price;
  static thread_local float cached_bid_qty;
  static thread_local float cached_ask_qty;

  static thread_local char str[] = "0000000000 0000000000 - 0000000000 0000000000\n";

  static thread_local char *str_bid_price = str + 0;
  static thread_local char *str_bid_qty = str + 11;
  static thread_local char *str_ask_price = str + 24;
  static thread_local char *str_ask_qty = str + 35;

  const bool bid_price_changed = top_of_book.best_bid_price != cached_bid_price;
  const bool ask_price_changed = top_of_book.best_ask_price != cached_ask_price;
  const bool bid_qty_changed = top_of_book.best_bid_qty != cached_bid_qty;
  const bool ask_qty_changed = top_of_book.best_ask_qty != cached_ask_qty;

  const bool anything_changed = bid_price_changed | ask_price_changed | bid_qty_changed | ask_qty_changed;
  if (!anything_changed) [[unlikely]]
    return;

  try_format<2>(bid_price_changed, str_bid_price, top_of_book.best_bid_price);
  try_format<2>(ask_price_changed, str_ask_price, top_of_book.best_ask_price);
  try_format<8>(bid_qty_changed, str_bid_qty, top_of_book.best_bid_qty);
  try_format<8>(ask_qty_changed, str_ask_qty, top_of_book.best_ask_qty);

  //TODO ideally this would be async. or at least accumulated and flushed every N changes
  write(STDOUT_FILENO, str, sizeof(str));

  cached_ask_price = top_of_book.best_ask_price;
  cached_bid_price = top_of_book.best_bid_price;
  cached_ask_qty = top_of_book.best_ask_qty;
  cached_bid_qty = top_of_book.best_bid_qty;
}

template <uint8_t Decimals>
void Logger::try_format(const bool changed, char *restrict buffer, const float value) noexcept
{
  using Handler = void (*)(char *, float);

  static constexpr Handler no_op = [](char *, float) {};
  static constexpr Handler format = [](char *restrict buffer, float value) {
    std::format_to(buffer, "{:.{}f}", value, Decimals);
  };

  static constexpr std::array<Handler, 2> handlers = {no_op, format};
  handlers[changed](buffer, value);
}