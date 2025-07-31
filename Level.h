#pragma once

#include "Order.h"
#include "Using.h"
#include <map>
#include <vector>

// TODO: Struct alignment, optimize padding here
struct Level {
  Price price;
  size_t num_orders;

  Order *tail;
  Order *head;
};

using Levels = std::vector<Level *>;
