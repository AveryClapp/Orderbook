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

  bool handle_buy(const Order &order);
  bool handle_sell(const Order &order);

public:
  Orderbook() = default;

  void receive_order(const Order &order);
}
