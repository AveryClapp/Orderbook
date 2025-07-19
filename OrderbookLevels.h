#pragma once

#include "LevelInfo.h"
#include "Using.h"

class OrderbookLevels {
private:
  LevelInfos<std::greater<Price>> bids_;
  LevelInfos<std::less<Price>> asks_;

public:
  const LevelInfos<std::greater<Price>> &get_bids() const { return bids_; }
  const LevelInfos<std::less<Price>> &get_asks() const { return asks_; }

  const void add_bid(const Order &bid);
  const void add_ask(const Order &ask);
}
