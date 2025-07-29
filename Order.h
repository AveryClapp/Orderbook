#pragma once

#include "Direction.h"
#include "OrderType.h"
#include "Using.h"
#include <cstdint>
#include <stdexcept>

struct Level;

struct Order {
  Price price_;
  OrderType type_;
  Direction direction_;
  Quantity initial_quantity_;
  Quantity remaining_quantity_;
  Time time_;
  Level *cur_level_;
  Order *next_order_;
  Order *prev_order_;
  ID id_;
}
