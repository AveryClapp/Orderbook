#pragma once

#include "include/core/Order.h"
#include "include/core/Using.h"
#include <boost/circular_buffer.hpp>
#include <boost/container/flat_map.hpp>
#include <cstddef>

struct Level {
  Price price;             // 4 bytes
  Quantity total_quantity; // 4 bytes
  boost::circular_buffer<Order *> orders{100};
};

template <typename Compare>
using Levels = boost::container::flat_map<Price, Level, Compare>;
