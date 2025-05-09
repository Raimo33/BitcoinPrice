/*================================================================================

File: GeminiMarketDataClient.cpp                                                                
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-03-08 15:48:16                                                 
last edited: 2025-05-03 19:03:11                                                

================================================================================*/

#include "GeminiMarketDataClient.hpp"
#include "utils/utils.hpp"
#include "macros.hpp"
#include "error.hpp"

//TODO error handling

COLD GeminiMarketDataClient::GeminiMarketDataClient(std::string_view pair) noexcept :
  io_ctx(),
  ssl_ctx(ssl::context::tlsv12_client),
  ws_stream(io_ctx, ssl_ctx),
  path("/v1/marketdata/" + std::string(pair))
{
  ssl_ctx.set_verify_mode(ssl::verify_none);
}

COLD GeminiMarketDataClient::~GeminiMarketDataClient() noexcept
{
}

void GeminiMarketDataClient::run(void) noexcept
{
  connect();
  listen();
}

COLD void GeminiMarketDataClient::connect(void)
{
  const std::string host = "api.gemini.com";
  const std::string port = "443";

  ip::tcp::resolver resolver(io_ctx);
  auto const results = resolver.resolve(host, port);

  net::connect(ws_stream.next_layer().next_layer(), results.begin(), results.end());
  ws_stream.next_layer().handshake(ssl::stream_base::client);
  ws_stream.handshake(host, path);
}

COLD void GeminiMarketDataClient::listen(void)
{

  while (true)
  {

  }


  std::unreachable();
}