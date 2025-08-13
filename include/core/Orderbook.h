#pragma once

#include "include/core/Message.h"
#include "include/core/Order.h"
#include "include/core/OrderbookLevels.h"
#include "include/core/Using.h"

class Orderbook {
private:
  OrderbookLevels levels_;

  std::unordered_map<ID, Order *> order_map_;

  void handle_buy(Order *order);
  void handle_sell(Order *order);
  void handle_cancel(const ID cancel_id);

public:
  Orderbook() = default;

  void receive_message(Message msg);

  void print_book();

  std::optional<std::pair<Price, size_t>> get_best_bid();
  std::optional<std::pair<Price, size_t>> get_best_ask();
};
