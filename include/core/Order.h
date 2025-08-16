#pragma once

#include "Direction.h"
#include "OrderType.h"
#include "Using.h"

struct Level;

struct Order {
  Price price;
  OrderType type;
  Direction direction;
  Quantity initial_quantity;
  Quantity remaining_quantity;
  Time time;
  Level *cur_level;
  ID id;
  size_t level_position;
};
