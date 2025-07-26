#pragma once

#include "Order.h"
#include "Using.h"
#include <map>

struct Level {
  Price price;
  Level *parent;
  Level *left_child;
  Level *right_child;
  Order *tail;
  Order *head;
};
