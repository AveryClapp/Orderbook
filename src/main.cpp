#include "include/core/Orderbook.h"
#include "include/core/Using.h"
#include <chrono>

int main() {
  Message msg_one = ({50, OrderType::Limit, Direction::Buy, 100, 100,
                      std::chrono::system_clock::now(), nullptr, nullptr});
  Orderbook orderbook;
  orderbook.receive_message(msg_one);
  orderbook.print_book();
  return 0;
}
