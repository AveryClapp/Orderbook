#pragma once

#include "include/core/Order.h"
#include "include/core/Using.h"
#include <boost/circular_buffer.hpp>
#include <boost/container/flat_map.hpp>
#include <cstddef>
#include <deque>
#include <map>

struct Level {
  Price price;             // 4 bytes
  Quantity total_quantity; // 4 bytes
  std::deque<Order *> orders;
};

template <typename Compare> using Levels = std::map<Price, Level, Compare>;
