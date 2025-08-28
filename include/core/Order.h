#pragma once

#include "Direction.h"
#include "OrderType.h"
#include "Using.h"
#include <list>

struct Level;
struct Order;

using ListIterator = std::list<Order *>::iterator;

struct Order {
  Price price;                // 4 Bytes
  Quantity quantity;          // 4 Bytes
  ID id;                      // 8 Bytes
  ListIterator list_iterator; // 8 bytes
  OrderType type;             // 1 Byte
  Direction direction;        // 1 Byte
};
