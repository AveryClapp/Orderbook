#pragma once

#include "Order.h"
#include "Using.h"
#include <map>
#include <vector>

struct Level {
  Price price;
  Level *parent;
  Level *left_child;
  Level *right_child;
  Order *tail;
  Order *head;
};

using Levels = std::vector<Level *>
