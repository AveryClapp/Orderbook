#pragma once

#include "Direction.h"
#include "OrderType.h"
#include "Using.h"
#include <cstdint>
#include <stdexcept>

struct Level;

struct Order {
  Price price;
  OrderType type;
  Direction direction;
  Quantity initial_quantity;
  Quantity remaining_quantity;
  Time time_;
  Level *cur_level;
  Order *next_order;
  Order *prev_order;
  ID id;
};
