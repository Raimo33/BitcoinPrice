/*================================================================================

File: Client.hpp                                                                
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-03-23 17:58:46                                                 
last edited: 2025-05-10 09:38:35                                                

================================================================================*/

#pragma once

#include <string_view>
#include <string>
#include <yyjson.h>
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

class Client
{
  public:

    Client(std::string_view pair) noexcept;
    ~Client() noexcept;

    void run(void) noexcept;

  private:

    void connect(void);
    void listen(void);

    static void processMarketData(std::string_view data);
    static void processOrder(yyjson_val *order);

    static void handleTrade(const OrderBook::Side side, const int32_t price, const uint64_t qty);
    static void handleChange(const OrderBook::Side side, const int32_t price, const uint64_t qty);

    net::io_context io_ctx;
    ssl::context ssl_ctx;
    websocket::stream<beast::ssl_stream<net::ip::tcp::socket>> ws_stream;

    std::string path;
    OrderBook order_book;
};