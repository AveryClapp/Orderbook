#pragma once
#include <cstdint>

enum class OrderType : uint8_t {
  FillOrKill,
  GoodTilCancel,
  GoodForDay,
};
