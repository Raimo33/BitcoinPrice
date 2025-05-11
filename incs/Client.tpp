/*================================================================================

File: Client.cpp                                                                
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-03-08 15:48:16                                                 
last edited: 2025-05-11 21:29:03                                                

================================================================================*/

#pragma once

#include <yyjson.h>

#include "Client.hpp"
#include "macros.hpp"
#include "utils.hpp"

template <uint8_t PriceDecimals, uint8_t QtyDecimals>
COLD Client<PriceDecimals, QtyDecimals>::Client(std::string_view pair) noexcept :
  ssl_ctx(ssl::context::tlsv12_client),
  ws_stream(io_ctx, ssl_ctx),
  path("/v1/marketdata/" + std::string(pair))
{
  ssl_ctx.set_verify_mode(ssl::verify_none);
}

template <uint8_t PriceDecimals, uint8_t QtyDecimals>
COLD Client<PriceDecimals, QtyDecimals>::~Client(void) noexcept
{
}

template <uint8_t PriceDecimals, uint8_t QtyDecimals>
void Client<PriceDecimals, QtyDecimals>::run(void) noexcept
{
  connect();
  listen();
  //TODO log best price, asynchronous
}

template <uint8_t PriceDecimals, uint8_t QtyDecimals>
COLD void Client<PriceDecimals, QtyDecimals>::connect(void)
{
  const std::string host = "api.gemini.com";
  const std::string port = "443";

  ip::tcp::resolver resolver(io_ctx);
  auto const results = resolver.resolve(host, port);

  net::connect(ws_stream.next_layer().next_layer(), results.begin(), results.end());

  auto &socket = beast::get_lowest_layer(ws_stream);
  socket.set_option(net::ip::tcp::no_delay(true));
  socket.set_option(net::socket_base::keep_alive(true));
  socket.native_non_blocking(true);

  //TODO remove
  socket.set_option(net::socket_base::reuse_address(true));
  socket.set_option(net::socket_base::keep_alive(true));

  ws_stream.next_layer().handshake(ssl::stream_base::client);
  ws_stream.handshake(host, path);
}

template <uint8_t PriceDecimals, uint8_t QtyDecimals>
HOT void Client<PriceDecimals, QtyDecimals>::listen(void)
{
  beast::flat_buffer buffer;
  buffer.reserve(8192);

  ws_stream.auto_fragment(false);
  ws_stream.binary(true);
  ws_stream.read_message_max(0);

  while (true)
  {
    const std::size_t read_bytes = ws_stream.read(buffer);

    //TODO better way
    std::string_view data{reinterpret_cast<const char*>(buffer.data().data()), read_bytes};
    processMarketData(data);

    buffer.consume(read_bytes);
  }
}

template <uint8_t PriceDecimals, uint8_t QtyDecimals>
HOT void Client<PriceDecimals, QtyDecimals>::processMarketData(std::string_view data)
{
  bool error = false;

  yyjson_doc *doc = yyjson_read_opts(const_cast<char*>(data.data()), data.size(), 0, nullptr, nullptr);
  error |= (doc == nullptr);

  yyjson_val *root = yyjson_doc_get_root(doc);
  error |= (root == nullptr);

  yyjson_val *events = yyjson_obj_get(root, "events");
  error |= (events == nullptr);
  error |= (yyjson_is_arr(events) == false);

  if (error) [[unlikely]]
    utils::throw_exception("Failed to parse JSON data");

  size_t idx, max;
  yyjson_val *event;
  yyjson_arr_foreach(events, idx, max, event)
    handleEvent(event);

  yyjson_doc_free(doc);
}

template <uint8_t PriceDecimals, uint8_t QtyDecimals>
HOT void Client<PriceDecimals, QtyDecimals>::handleEvent(yyjson_val *event)
{
  using Handler = void (Client<PriceDecimals, QtyDecimals>::*)(yyjson_val *);

  static constexpr uint8_t size = 't' + 1;
  static constexpr std::array<Handler, size> handlers = [](){
    std::array<Handler, size> handlers{};
    handlers['c'] = &Client<PriceDecimals, QtyDecimals>::handleChange;
    handlers['t'] = &Client<PriceDecimals, QtyDecimals>::handleTrade;
    return handlers;
  }();

  const char type = yyjson_get_str(event)[0];
  (this->*handlers[type])(event);
}

template <uint8_t PriceDecimals, uint8_t QtyDecimals>
HOT void Client<PriceDecimals, QtyDecimals>::handleChange(yyjson_val *event)
{
  yyjson_val *side_obj = yyjson_obj_get(event, "side");
  yyjson_val *price_obj = yyjson_obj_get(event, "price");
  yyjson_val *qty_obj = yyjson_obj_get(event, "remaining");

  const char *side_str = yyjson_get_str(side_obj);

  //TODO optimize conversion
  const char side = side_str[0];
  const PriceType price(yyjson_get_real(price_obj));
  const QtyType qty(yyjson_get_real(qty_obj));

  //TODO make branchless (hard to predict 50/50)
  if (side == 'b')
    order_book.setBidQty(price, qty);
  else
    order_book.setAskQty(price, qty);
}

template <uint8_t PriceDecimals, uint8_t QtyDecimals>
HOT void Client<PriceDecimals, QtyDecimals>::handleTrade(UNUSED yyjson_val *event)
{
}