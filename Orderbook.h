#pragma once

#include "LevelInfo.h"
#include "Order.h"
#include "OrderbookLevels.h"
#include "Using.h"
#include <algorithm>
#include <iostream>
#include <vector>

class Orderbook {
private:
  OrderbookLevels m_levels

      void
      update_level_info(const Order &order);

public:
  Orderbook() = default;

  void receive_order(const Order &order);
}
