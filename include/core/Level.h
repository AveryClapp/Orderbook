#pragma once

#include "include/core/Order.h"
#include "include/core/Using.h"
#include <boost/container/flat_map.hpp>
#include <queue>

struct Level {
  Price price;
  std::queue<Order *> orders;
};

template <typename Compare>
using Levels = boost::container::flat_map<Price, Level, Compare>;
