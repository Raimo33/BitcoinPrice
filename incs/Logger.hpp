/*================================================================================

File: Logger.hpp                                                                
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-05-13 14:30:09                                                 
last edited: 2025-05-13 14:30:09                                                

================================================================================*/

#pragma once

#include "ipq/SPSCQueue.hpp"
#include "Client.hpp"

template <uint8_t PriceDecimals, uint8_t QtyDecimals>
class Logger
{
  public:

    Logger(std::string_view pair) noexcept;
    ~Logger() noexcept;

    void start(void);

  private:

    using PriceType = FixedPoint<PriceDecimals>;
    using QtyType = FixedPoint<QtyDecimals>;

    struct alignas(CACHELINE_SIZE) TopOfBook
    {
      PriceType best_bid_price;
      PriceType best_ask_price;
      QtyType best_bid_qty;
      QtyType best_ask_qty;

      bool operator==(const TopOfBook& other) const noexcept;
    };

    std::string pair;
    ipq::SPSCQueue<TopOfBook, 64> queue;

    void log(const TopOfBook& top_of_book);
};

#include "Logger.tpp"