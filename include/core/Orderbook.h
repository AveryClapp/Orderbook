#pragma once

#include "include/core/Message.h"
#include "include/core/Order.h"
#include "include/core/OrderPool.h"
#include "include/core/OrderbookLevels.h"
#include "include/core/Using.h"
#include <optional>
#include <unordered_map>

#define POOL_SIZE 999999

// Custom hash since all IDs are unique
struct IDHash {
  size_t operator()(ID id) const noexcept { return static_cast<size_t>(id); }
};

class Orderbook {
private:
  OrderbookLevels levels_;
  OrderPool order_pool_;

  std::unordered_map<ID, Order *, IDHash> order_map_;

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
