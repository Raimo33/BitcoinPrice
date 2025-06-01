/*================================================================================

File: Client.hpp                                                                
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-03-23 17:58:46                                                 
last edited: 2025-05-13 14:30:09                                                

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
#include <ipq/SPSCQueue.hpp>

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
    Client(std::string_view pair);
    ~Client() noexcept;

    void run(void);

  private:
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    using PriceType = FixedPoint<PriceDecimals>;
    using QtyType = FixedPoint<QtyDecimals>;

    struct alignas(CACHELINE_SIZE) TopOfBook
    {
      PriceType best_bid_price;
      PriceType best_ask_price;
      QtyType best_bid_qty;
      QtyType best_ask_qty;
    };

    net::io_context _io_ctx;
    ssl::context _ssl_ctx;
    websocket::stream<beast::ssl_stream<net::ip::tcp::socket>> _ws_stream;
    std::string _pair;
    const int _shared_fd;
    ipq::SPSCQueue<TopOfBook, 64> _queue;
    OrderBook<PriceType, QtyType> _order_book;

    void connect(void);
    void listen(void);

    void processMarketData(std::string_view data);

    void handleEvent(yyjson_val *event);
    void handleChange(yyjson_val *event);
    void handleTrade(yyjson_val *event);

    void broadcastTopOfBook(void);
};

#include "Client.tpp"