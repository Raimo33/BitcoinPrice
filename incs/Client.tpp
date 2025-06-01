/*================================================================================

File: Client.cpp                                                                
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-03-08 15:48:16                                                 
last edited: 2025-05-13 14:30:09                                                

================================================================================*/

#pragma once

#include <yyjson.h>

#include "Client.hpp"
#include "macros.hpp"
#include "utils.hpp"

template <uint8_t PriceDecimals, uint8_t QtyDecimals>
COLD Client<PriceDecimals, QtyDecimals>::Client(std::string_view pair) :
  _ssl_ctx(ssl::context::tlsv12_client),
  _ws_stream(_io_ctx, _ssl_ctx),
  _pair(pair),
  _shared_fd(utils::get_shared_memory_fd(pair)),
  _queue(_shared_fd)
{
  _ssl_ctx.set_verify_mode(ssl::verify_none);
}

template <uint8_t PriceDecimals, uint8_t QtyDecimals>
COLD Client<PriceDecimals, QtyDecimals>::~Client(void) noexcept
{
  close(_shared_fd);
}

template <uint8_t PriceDecimals, uint8_t QtyDecimals>
void Client<PriceDecimals, QtyDecimals>::run(void)
{
  connect();
  listen();
}

template <uint8_t PriceDecimals, uint8_t QtyDecimals>
COLD void Client<PriceDecimals, QtyDecimals>::connect(void)
{
  const std::string host = "api.gemini.com";
  const std::string port = "443";
  const std::string path = "/v1/marketdata/" + _pair;

  ip::tcp::resolver resolver(_io_ctx);
  auto const results = resolver.resolve(host, port);

  net::connect(_ws_stream.next_layer().next_layer(), results.begin(), results.end());

  auto &socket = beast::get_lowest_layer(_ws_stream);
  socket.set_option(net::ip::tcp::no_delay(true));
  socket.set_option(net::socket_base::keep_alive(true));
  socket.native_non_blocking(true);

  //TODO remove
  socket.set_option(net::socket_base::reuse_address(true));
  socket.set_option(net::socket_base::keep_alive(true));

  _ws_stream.next_layer().handshake(ssl::stream_base::client);
  _ws_stream.handshake(host, path);
}

template <uint8_t PriceDecimals, uint8_t QtyDecimals>
HOT void Client<PriceDecimals, QtyDecimals>::listen(void)
{
  beast::flat_buffer buffer;
  buffer.prepare(16 * 1024);

  _ws_stream.auto_fragment(false);
  _ws_stream.binary(true);
  _ws_stream.read_message_max(0);

  while (true)
  {
    const std::size_t read_bytes = _ws_stream.read(buffer);

    //TODO find a more readable way
    std::string_view data{static_cast<const char*>(buffer.data().data()), read_bytes};
    processMarketData(data);

    buffer.consume(read_bytes);
  }

  // TODO std::unreachable(); in c++23
}

//TODO: YYJSON improvement proposal: parse only needed JSON fields, pre-set at compile time
template <uint8_t PriceDecimals, uint8_t QtyDecimals>
HOT void Client<PriceDecimals, QtyDecimals>::processMarketData(std::string_view data)
{
  yyjson_doc *doc = yyjson_read_opts(const_cast<char*>(data.data()), data.size(), 0, nullptr, nullptr);
  yyjson_val *root = yyjson_doc_get_root(doc);
  yyjson_obj_iter iter = yyjson_obj_iter_with(root);
  yyjson_val *events = yyjson_obj_iter_get(&iter, "events");

  if (!yyjson_is_arr(events)) [[unlikely]]
    utils::throw_exception("Failed to parse JSON data");

  size_t idx, max;
  yyjson_val *event;
  yyjson_arr_foreach(events, idx, max, event)
    handleEvent(event);

  yyjson_doc_free(doc);
}

template <uint8_t PriceDecimals, uint8_t QtyDecimals>
HOT void Client<PriceDecimals, QtyDecimals>::handleEvent(yyjson_val *restrict event)
{
  using Handler = void (Client<PriceDecimals, QtyDecimals>::*)(yyjson_val *);

  static constexpr uint8_t size = 't' + 1;
  static constexpr std::array<Handler, size> handlers = [](){
    std::array<Handler, size> handlers{};
    handlers['c'] = &Client<PriceDecimals, QtyDecimals>::handleChange;
    handlers['t'] = &Client<PriceDecimals, QtyDecimals>::handleTrade;
    return handlers;
  }();

  yyjson_obj_iter iter = yyjson_obj_iter_with(event);
  yyjson_val *type_obj = yyjson_obj_iter_get(&iter, "type");
  const char *type_str = yyjson_get_str(type_obj);

  const char type = type_str[0];
  (this->*handlers[type])(event);
}

template <uint8_t PriceDecimals, uint8_t QtyDecimals>
HOT void Client<PriceDecimals, QtyDecimals>::handleChange(yyjson_val *restrict event)
{
  yyjson_obj_iter iter = yyjson_obj_iter_with(event);

  yyjson_val *restrict side_obj = yyjson_obj_iter_get(&iter, "side");
  yyjson_val *restrict price_obj = yyjson_obj_iter_get(&iter, "price");
  yyjson_val *restrict qty_obj = yyjson_obj_iter_get(&iter, "remaining");

  const char *restrict side_str = yyjson_get_str(side_obj);
  const char *restrict price_str = yyjson_get_str(price_obj);
  const char *restrict qty_str = yyjson_get_str(qty_obj);

  const char side = side_str[0];
  const PriceType price(price_str);
  const QtyType qty(qty_str);

  //TODO make branchless (hard to predict 50/50)
  if (side == 'b')
    _order_book.setBidQty(price, qty);
  else
    _order_book.setAskQty(price, qty);

  broadcastTopOfBook();
}

template <uint8_t PriceDecimals, uint8_t QtyDecimals>
HOT void Client<PriceDecimals, QtyDecimals>::handleTrade(UNUSED yyjson_val *restrict event)
{
}

template <uint8_t PriceDecimals, uint8_t QtyDecimals>
HOT void Client<PriceDecimals, QtyDecimals>::broadcastTopOfBook(void)
{
  TopOfBook msg{
    _order_book.getBestBidPrice(),
    _order_book.getBestAskPrice(),
    _order_book.getBestBidQty(),
    _order_book.getBestAskQty()
  };

  _queue.push(std::move(msg));
}