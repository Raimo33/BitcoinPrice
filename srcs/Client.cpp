/*================================================================================

File: Client.cpp                                                                
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-03-08 15:48:16                                                 
last edited: 2025-05-09 20:42:19                                                

================================================================================*/

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
  socket.non_blocking(true);
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

    std::string_view data{
      reinterpret_cast<const char*>(buffer.data().data()),
      buffer.data().size()
    };
    process_market_data(data);

    buffer.consume(buffer.size());
  }
}

HOT void Client::process_market_data(std::string_view data)
{
  printf("%s\n", data.data());
}