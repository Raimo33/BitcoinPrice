/*================================================================================

File: Logger.hpp                                                                
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-05-13 14:30:09                                                 
last edited: 2025-05-13 16:40:17                                                

================================================================================*/

#pragma once

#include "ipq/SPSCQueue.hpp"
#include "FixedPoint.hpp"
#include "Messages.hpp"

template <uint8_t PriceDecimals, uint8_t QtyDecimals>
class Logger
{
  public:
    Logger(std::string_view pair) noexcept;
    ~Logger() noexcept;

    void start(void);

  private:
    Logger(const Logger &) = delete;
    Logger& operator=(const Logger &) = delete;

    using PriceType = FixedPoint<PriceDecimals>;
    using QtyType = FixedPoint<QtyDecimals>;
    using TopOfBook = messages::TopOfBook<PriceType, QtyType>;

    std::string _pair;
    const int _shared_fd;
    ipq::SPSCQueue<TopOfBook, 64> _queue;

    void try_log(const TopOfBook &top_of_book);

    template <uint8_t Decimals>
    static void try_format(const bool changed, char *restrict buffer, const FixedPoint<Decimals> &fixed_point) noexcept;
};

#include "Logger.tpp"