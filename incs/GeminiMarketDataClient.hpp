/*================================================================================

File: Client.hpp                                                                
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-03-23 17:58:46                                                 
last edited: 2025-04-08 19:45:06                                                

================================================================================*/

#pragma once

#include <string_view>
#include <string>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/ssl.hpp>

#include "OrderBook.hpp"

namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
namespace beast = boost::beast;
namespace ip = boost::asio::ip;
namespace websocket = beast::websocket;

class GeminiMarketDataClient
{
  public:

    GeminiMarketDataClient(std::string_view pair) noexcept;
    ~GeminiMarketDataClient() noexcept;

    void run(void) noexcept;

  private:

    net::io_context io_ctx;
    ssl::context ssl_ctx;
    websocket::stream<beast::ssl_stream<net::ip::tcp::socket>> ws_stream;

    std::string path;
    OrderBook order_book;

    void connect(void);
    void listen(void);
};