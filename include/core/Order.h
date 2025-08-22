#pragma once

#include "Direction.h"
#include "OrderType.h"
#include "Using.h"

struct Level;

struct Order {
  Price price;                 // 4 Bytes
  Quantity initial_quantity;   // 4 Bytes
  Quantity remaining_quantity; // 4 Bytes
  ID id;                       // 8 Bytes
  size_t level_position;       // 8 Bytes
  Time time;                   // 8 bytes
  Level *cur_level;            // 30 + Bytes
  OrderType type;              // 1 Byte
  Direction direction;         // 1 Byte
};
