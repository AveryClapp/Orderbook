#pragma once

#include "Level.h"
#include "Message.h"
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

  void update_level_info(Order &order);

  void handle_buy(Order *order);
  void handle_sell(Order *order);

public:
  Orderbook() = default;

  void receive_message(Order *order);
};
