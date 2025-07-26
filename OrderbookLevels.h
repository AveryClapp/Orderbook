#pragma once

#include "LevelInfo.h"
#include "Order.h"
#include "Using.h"
#include <unordered_map>

class OrderbookLevels {
private:
  LevelInfo *bids_;
  LevelInfo *asks_;

  // O(1) Lookup `
  std::unordered_map<ID, Order *> order_map_;
  std::unordered_map<Price, LevelInfo> level_info_map_;

public:
  LevelInfo *get_bids() { return bids_; }
  LevelInfo *get_asks() { return asks_; }

  const void add_bid(const Order &bid);
  const void add_ask(const Order &ask);

  const void cancel_bid(const ID id);
  const void cancel_ask(const ID id);
};
