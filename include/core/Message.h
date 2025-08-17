#pragma once
#include "include/core/Direction.h"
#include "include/core/OrderType.h"
#include "include/core/Using.h"

#include <variant>

struct NewOrderData {
  ID id;
  Price price;
  Quantity quantity;
  Direction direction;
  OrderType type;
};

struct CancelData {
  ID order_id;
};

struct Message {
  std::variant<NewOrderData, CancelData> data;
};
