#pragma once

#include "include/core/Level.h"
#include "include/core/Order.h"
#include "include/core/Using.h"
#include <stdexcept>
#include <unordered_map>

class OrderbookLevels {
private:
  Levels<std::greater<Price>> bids_;
  Levels<std::less<Price>> asks_;

public:
  auto get_bids() { return bids_; }
  auto get_asks() { return asks_; }

  void add_bid(Order *bid);
  void add_ask(Order *ask);
};
