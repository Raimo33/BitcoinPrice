/*================================================================================

File: utils.cpp                                                                 
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-05-10 11:37:51                                                 
last edited: 2025-05-13 14:30:09                                                

================================================================================*/

#include <iostream>
#include <stdexcept>
#include <boost/exception/all.hpp>
#include <csignal>

#include "utils.hpp"
#include "macros.hpp"

namespace utils
{

[[noreturn]] COLD NEVER_INLINE void throw_exception(const std::string_view msg)
{
#ifdef __EXCEPTIONS
  throw std::runtime_error(std::string(msg));
#else
  std::cerr << msg << std::endl;
  std::terminate();
#endif
}

COLD void setup_signal_handler(void)
{
  struct sigaction sa{};

  sa.sa_handler = [](int) { std::terminate(); };

  bool error = false;

  error |= sigaction(SIGINT, &sa, nullptr) == -1;
  error |= sigaction(SIGTERM, &sa, nullptr) == -1;
  error |= sigaction(SIGQUIT, &sa, nullptr) == -1;
  error |= sigaction(SIGPIPE, &sa, nullptr) == -1;

  if (error)
    throw_exception("Failed to set signal handler");
}

}

#ifndef __EXCEPTIONS
COLD NEVER_INLINE void boost::throw_exception(UNUSED const std::exception &e)
{
  std::abort();
}

COLD NEVER_INLINE void boost::throw_exception(UNUSED const std::exception &e, UNUSED const boost::source_location &loc)
{
  std::abort();
}
#endif