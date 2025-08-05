#pragma once
#include "include/core/Cancel.h"
#include "include/core/Order.h"

#include <variant>

struct Message {
  std::variant<Cancel, Order *> data;
};
