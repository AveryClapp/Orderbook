#pragma once

#include "Using.h"
#include <map>

struct LevelInfo {
  Price price;
  Quantity quantity;
};

// LevelInfos is a map sorted by price;
template <typename Comparator>
using LevelInfos = std::map<Price, Quantity, Comparator>;
