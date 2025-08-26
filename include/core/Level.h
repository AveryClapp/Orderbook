#pragma once

#include "include/core/Order.h"
#include "include/core/RingBuffer.h"
#include "include/core/Using.h"
#include <boost/container/flat_map.hpp>
#include <cstddef>

struct Level {
  Price price;             // 4 bytes
  Quantity total_quantity; // 4 bytes
  RingBuffer<> orders;     // A lot of bytes.
};

template <typename Compare>
using Levels = boost::container::flat_map<Price, Level, Compare>;
