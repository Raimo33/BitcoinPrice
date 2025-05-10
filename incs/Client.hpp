/*================================================================================

File: Client.hpp                                                                
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-03-23 17:58:46                                                 
last edited: 2025-05-10 22:09:22                                                

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
#include "FixedPoint.hpp"

namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
namespace beast = boost::beast;
namespace ip = boost::asio::ip;
namespace websocket = beast::websocket;

template <uint8_t PriceDecimals, uint8_t QtyDecimals>
class Client
{
  public:

    Client(std::string_view pair) noexcept;
    ~Client() noexcept;

    void run(void) noexcept;

  private:

    using PriceType = FixedPoint<PriceDecimals>;
    using QtyType = FixedPoint<QtyDecimals>;

    net::io_context io_ctx;
    ssl::context ssl_ctx;
    websocket::stream<beast::ssl_stream<net::ip::tcp::socket>> ws_stream;

    std::string path;
    OrderBook<PriceType, QtyType> order_book;

    void connect(void);
    void listen(void);

    void processMarketData(std::string_view data);
    void processOrder(yyjson_val *order);
    void handleChange(const char side, const PriceType, const QtyType qty);
};

#include "Client.tpp"