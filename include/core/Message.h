#include "include/core/Cancel.h"
#include <variant>

struct Message {
  std::variant<Cancel, Order> data;
};
