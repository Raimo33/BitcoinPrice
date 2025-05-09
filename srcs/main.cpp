/*================================================================================

File: main.cpp                                                                  
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-03-08 18:21:38                                                 
last edited: 2025-04-24 12:40:43                                                

================================================================================*/

#include <csignal>

#include "Client.hpp"
#include "utils/utils.hpp"

void init_signal_handler(void);

int main(void)
{
  init_signal_handler();

  Client client("BTCUSD");
  client.run();
}

COLD void init_signal_handler(void)
{
  struct sigaction sa{};

  sa.sa_handler = [](int) { std::terminate(); };

  bool error = false;

  error |= sigaction(SIGINT, &sa, nullptr) == -1;
  error |= sigaction(SIGTERM, &sa, nullptr) == -1;
  error |= sigaction(SIGQUIT, &sa, nullptr) == -1;
  error |= sigaction(SIGPIPE, &sa, nullptr) == -1;

  if (error)
    utils::throw_exception("Failed to set signal handler");
}