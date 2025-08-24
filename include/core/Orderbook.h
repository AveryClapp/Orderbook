#pragma once

#include "include/core/Message.h"
#include "include/core/Order.h"
#include "include/core/OrderPool.h"
#include "include/core/OrderbookLevels.h"
#include "include/core/Using.h"
#include <unordered_map>
#include <optional>


#define POOL_SIZE 10000

class Orderbook {
private:
  OrderbookLevels levels_;
  OrderPool order_pool_;

  std::unordered_map<ID, Order *> order_map_;

  void handle_buy(Order *order);
  void handle_sell(Order *order);
  void handle_cancel(const ID cancel_id);

  bool can_fill(const NewOrderData &order);

public:
  Orderbook();

  void receive_message(const Message &msg);

  void print_book();

  std::optional<std::pair<Price, size_t>> get_best_bid();
  std::optional<std::pair<Price, size_t>> get_best_ask();
};
