#pragma once

#include "include/core/Order.h"
#include "include/core/Using.h"
#include <boost/container/flat_map.hpp>
#include <deque>

struct Level {
  Price price;
  std::deque<Order *> orders;

  void cancel_order(size_t position) {
    if (position >= orders.size()) {
      throw std::runtime_error("Invalid position");
    }

    orders.erase(orders.begin() + position);

    for (size_t i = position; i < orders.size(); i++) {
      orders[i]->level_position = i;
    }
  }
};

template <typename Compare>
using Levels = boost::container::flat_map<Price, Level, Compare>;
