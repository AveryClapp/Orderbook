#pragma once
#include "include/core/Message.h"
#include "include/core/Order.h"

namespace test_utils {
Order *create_order(ID id, Price price, Quantity qty, Direction dir,
                    OrderType type = OrderType::GoodTilCancel);

Message create_order_message(ID id, Price price, Quantity qty, Direction dir,
                             OrderType type = OrderType::GoodTilCancel);
Message create_cancel_message(ID id);

}; // namespace test_utils
