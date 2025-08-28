#pragma once
#include "include/core/Message.h"

namespace test_utils {
NewOrderData create_order(ID id, Price price, Quantity qty, Direction dir);

CancelData create_cancel_message(ID id);
}; // namespace test_utils
