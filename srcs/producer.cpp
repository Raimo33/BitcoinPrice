/*================================================================================

File: main.cpp                                                                  
Creator: Claudio Raimondi                                                       
Email: claudio.raimondi@pm.me                                                   

created at: 2025-03-08 18:21:38                                                 
last edited: 2025-05-13 16:40:17                                                

================================================================================*/

#include "Client.hpp"
#include "utils.hpp"

int main(void)
{
  utils::setup_signal_handler();

  Client<2, 8> client("BTCUSDT");
  client.run();
}