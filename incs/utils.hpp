/*================================================================================

File: utils.hpp                                                                 
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-04-03 20:16:29                                                 
last edited: 2025-05-13 16:40:17                                                

================================================================================*/

#pragma once

#include <string_view>

#include "FixedPoint.hpp"

namespace utils
{
  [[noreturn]] void throw_exception(const std::string_view msg);
  void setup_signal_handler(void);
}