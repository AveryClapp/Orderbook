#include "Cancel.h"
#include <variant>

struct Message {
  std::variant<Cancel, Order> data;
}
