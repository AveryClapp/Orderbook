#pragma once

#include "LevelInfo.h"
#include "Order.h"
#include "Using.h"

class OrderbookLevels {
private:
  LevelInfos<std::greater<Price>> bids_;
  LevelInfos<std::less<Price>> asks_;

public:
  LevelInfos<std::greater<Price>> &get_bids() { return bids_; }
  LevelInfos<std::less<Price>> &get_asks() { return asks_; }

  const void add_bid(const Order &bid);
  const void add_ask(const Order &ask);
};
