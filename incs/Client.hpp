/*================================================================================

File: Client.hpp                                                                
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-03-23 17:58:46                                                 
last edited: 2025-05-12 10:11:38                                                

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
#include <ipqueue/SPSCQueue.hpp>

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
    ipqueue::SPSCQueue queue;

    std::string path;
    OrderBook<PriceType, QtyType> order_book;

    struct alignas(CACHELINE_SIZE) TopOfBook
    {
      PriceType best_bid_price;
      PriceType best_ask_price;
      QtyType best_bid_qty;
      QtyType best_ask_qty;
    };

    void connect(void);
    void listen(void);

    void processMarketData(std::string_view data);

    void handleEvent(yyjson_val *event);
    void handleChange(yyjson_val *event);
    void handleTrade(yyjson_val *event);

    void broadcastTopOfBook(void) const;
};

#include "Client.tpp"