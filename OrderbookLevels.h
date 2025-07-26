#pragma once

#include "Level.h"
#include "Order.h"
#include "Using.h"
#include <unordered_map>

class OrderbookLevels {
private:
  std::unique_ptr<Level> bids_;
  std::unique_ptr<Level> asks_;

  // O(1) Lookup `
  std::unordered_map<ID, Order *> order_map_;
  std::unordered_map<Price, Level *> level_map_;

  void add_level(Order *order);

public:
  Level *get_bids() { return bids_; }
  Level *get_asks() { return asks_; }

  const void add_bid(const Order &bid);
  const void add_ask(const Order &ask);

  const void cancel_bid(const ID id);
  const void cancel_ask(const ID id);
};
