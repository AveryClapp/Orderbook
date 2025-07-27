#pragma once

#include "Level.h"
#include "Order.h"
#include "Using.h"
#include <stdexcept>
#include <unordered_map>

class OrderbookLevels {
private:
  Level *bids_;
  Level *asks_;

  // O(1) Lookup `
  std::unordered_map<ID, Order *> order_map_;
  std::unordered_map<Price, Level *> level_map_;

  void add_level(Order *order);
  void remove_level(Order *order);

public:
  Level *get_bids() { return bids_; }
  Level *get_asks() { return asks_; }

  void add_bid(Order *bid);
  void add_ask(Order *ask);

  void cancel_order(const ID id);
};
