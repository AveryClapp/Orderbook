#pragma once

#include "LevelInfo.h"
#include "Order.h"
#include "OrderbookLevels.h"
#include "Using.h"
#include <algorithm>
#include <exception>
#include <iostream>
#include <vector>

class Orderbook {
private:
  OrderbookLevels levels_;

  void update_level_info(const Order &order);

  void handle_buy(const Order &order);
  void handle_sell(const Order &order);

  void add_bid(const Order &bid);
  void add_ask(const Order &ask);

public:
  Orderbook() = default;

  void receive_order(const Order &order);
}
