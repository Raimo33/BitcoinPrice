/*================================================================================

File: Client.cpp                                                                
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-03-08 15:48:16                                                 
last edited: 2025-05-10 22:09:22                                                

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
    ws_stream.read(buffer);

    auto seq = buffer.data();
    std::string_view data{reinterpret_cast<const char*>(seq.data()), seq.size()};
    processMarketData(data);

    buffer.consume(buffer.size());
  }
}

template <uint8_t PriceDecimals, uint8_t QtyDecimals>
HOT void Client<PriceDecimals, QtyDecimals>::processMarketData(std::string_view data)
{
  bool error = false;

  yyjson_doc *doc = yyjson_read_opts(const_cast<char*>(data.data()), data.size(), YYJSON_READ_ALLOW_TRAILING_COMMAS, nullptr, nullptr);
  error |= (doc == nullptr);

  yyjson_val *root = yyjson_doc_get_root(doc);
  error |= (root == nullptr);

  yyjson_val *events = yyjson_obj_get(root, "events");
  error |= (events == nullptr);
  error |= (yyjson_is_arr(events) == false);

  if (error) [[unlikely]]
    utils::throw_exception("Failed to parse JSON data");

  size_t idx, max;
  yyjson_val *order;
  yyjson_arr_foreach(events, idx, max, order)
    processOrder(order);

  yyjson_doc_free(doc);
}

template <uint8_t PriceDecimals, uint8_t QtyDecimals>
HOT void Client<PriceDecimals, QtyDecimals>::processOrder(yyjson_val *order)
{
  yyjson_val *type_obj = yyjson_obj_get(order, "type");
  yyjson_val *side_obj = yyjson_obj_get(order, "side");
  yyjson_val *price_obj = yyjson_obj_get(order, "price");
  yyjson_val *qty_obj = yyjson_obj_get(order, "remaining");

  const char *type_str = yyjson_get_str(type_obj);

  const bool is_change = (type_str[0] == 'c');
  if (!is_change)
    return;

  const char *side_str = yyjson_get_str(side_obj);

  //TODO optimize conversion
  const char side = side_str[0];
  const PriceType price(yyjson_get_real(price_obj));
  const QtyType qty(yyjson_get_real(qty_obj));

  handleChange(side, price, qty);
}

#include <iostream>
template <uint8_t PriceDecimals, uint8_t QtyDecimals>
HOT void Client<PriceDecimals, QtyDecimals>::handleChange(const char side, const PriceType price, const QtyType qty)
{
  std::cout << "Side: " << side << ", Price: " << price << ", Qty: " << qty << std::endl;

  //TODO make branchless (hard to predict 50/50)
  if (side == 'b')
    order_book.setBidQty(price, qty);
  else
    order_book.setAskQty(price, qty);
}