#pragma once

#include "LevelInfo.h"
#include "Using.h"

class OrderbookLevels {
private:
  LevelInfos bids_;
  LevelInfos asks_;

public:
  const LevelInfos &get_bids() const { return m_bids; }
  const LevelInfos &get_asks() const { return m_asks; }

  const void add_bid(const Order &bid);
  const void add_ask(const Order &ask);
}
