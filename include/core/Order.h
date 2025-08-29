#pragma once

#include "Direction.h"
#include "Using.h"
#include <list>

struct Level;
struct Order;

using ListIterator = std::list<Order *>::iterator;

struct Order {
  ID id;                 // 8 Bytes
  size_t level_position; // 8 Byte
  Price price;           // 4 Bytes
  Quantity quantity;     // 4 Bytes
  Direction direction;   // 1 Byte
};
