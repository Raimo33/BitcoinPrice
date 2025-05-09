/*================================================================================

File: Client.cpp                                                                
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-03-08 15:48:16                                                 
last edited: 2025-05-03 19:03:11                                                

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
  ws_stream.next_layer().handshake(ssl::stream_base::client);
  ws_stream.handshake(host, path);
}

COLD void Client::listen(void)
{
  beast::flat_buffer buffer;

  while (ws_stream.is_open())
  {
    ws_stream.read(buffer);

    if (buffer.size() == 0)
      break;

    const auto json = beast::buffers_to_string(buffer.data());

    printf("%s\n", json.c_str());

    buffer.consume(buffer.size());
  }

  std::unreachable();
}