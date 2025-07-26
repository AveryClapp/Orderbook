#pragma once

#include "Order.h"
#include "Using.h"
#include <map>

struct LevelInfo {
  Price price;
  LevelInfo *parent;
  LevelInfo *left_child;
  LevelInfo *right_child;
  Order *tail;
  Order *head;
};
