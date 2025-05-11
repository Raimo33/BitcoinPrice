/*================================================================================

File: utils.cpp                                                                 
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-05-10 11:37:51                                                 
last edited: 2025-05-10 11:37:51                                                

================================================================================*/

#include <iostream>
#include <stdexcept>
#include <boost/exception/all.hpp>

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