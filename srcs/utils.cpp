#include <iostream>

#include "utils/utils.hpp"

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