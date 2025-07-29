#pragma once

#include "Level.h"
#include "Order.h"
#include "Using.h"
#include <stdexcept>
#include <unordered_map>

class OrderbookLevels {
private:
  Levels bids_;
  Levels asks_;

  std::unordered_map<ID, Order *> order_map_;
  std::unordered_map<Price, Level *> bid_level_map_;
  std::unordered_map<Price, Level *> ask_level_map_;

  void add_ask_level(Order *ask);
  void add_bid_level(Order *bid);

public:
  Levels &get_bids() { return bids_; }
  Levels &get_asks() { return asks_; }

  void add_bid(Order *bid);
  void add_ask(Order *ask);

  void cancel_order(const ID id);
};
