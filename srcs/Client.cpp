/*================================================================================

File: Client.cpp                                                                
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-03-08 15:48:16                                                 
last edited: 2025-05-10 09:38:35                                                

================================================================================*/

#include <yyjson.h>

#include "Client.hpp"
#include "utils/utils.hpp"
#include "macros.hpp"

COLD Client::Client(std::string_view pair) noexcept :
  io_ctx(),
  ssl_ctx(ssl::context::tlsv12_client),
  ws_stream(io_ctx, ssl_ctx),
  path("/v1/marketdata/" + std::string(pair))
{
  ssl_ctx.set_verify_mode(ssl::verify_none);
}

COLD Client::~Client() noexcept
{
}

void Client::run(void) noexcept
{
  connect();
  listen();
}

COLD void Client::connect(void)
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

HOT void Client::listen(void)
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

HOT void Client::processMarketData(std::string_view data)
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

HOT void Client::processOrder(yyjson_val *order)
{
  const char *type_str = yyjson_get_str(yyjson_obj_get(order, "type"));
  const char *side_str = yyjson_get_str(yyjson_obj_get(order, "side"));
  const char *price_str = yyjson_get_str(yyjson_obj_get(order, "price"));
  const char *qty_str = yyjson_get_str(yyjson_obj_get(order, "delta"));

  const OrderBook::Side side = static_cast<OrderBook::Side>(side_str[0] == 'a');
  const int32_t price = 100; //TODO get actual price, float to int32_t
  const uint64_t qty = 100; //TODO get actual qty

  using Handler = void (*)(const OrderBook::Side, const int32_t, const uint64_t);
  static constexpr std::array<Handler, 2> handlers = {
    &Client::handleChange,
    &Client::handleTrade
  };

  const bool is_trade = (type_str[0] == 't');
  handlers[is_trade](side, price, qty);
}

HOT void Client::handleTrade(const OrderBook::Side side, const int32_t price, const uint64_t qty)
{
  printf("received trade: %s %d %lu\n", side == OrderBook::BID ? "BID" : "ASK", price, qty);
}

HOT void Client::handleChange(const OrderBook::Side side, const int32_t price, const uint64_t qty)
{
  printf("received change: %s %d %lu\n", side == OrderBook::BID ? "BID" : "ASK", price, qty);
}