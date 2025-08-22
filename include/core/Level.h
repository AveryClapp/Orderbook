#pragma once

#include "include/core/Order.h"
#include "include/core/Using.h"
#include <boost/container/flat_map.hpp>
#include <cstddef>
#include <deque>

struct Level {
  Price price;             // 4 bytes
  Quantity total_quantity; // 4 bytes
  std::deque<Order *> orders;
};

template <typename Compare>
using Levels = boost::container::flat_map<Price, Level, Compare>;
