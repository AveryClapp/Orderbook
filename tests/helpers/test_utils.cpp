#include "tests/helpers/test_utils.h"

Order TestOrderFactory::create_order(ID id, Price price, Quantity qty,
                                     Direction dir) {
  return Order{.price = price,
               .type = OrderType::Limit,
               .direction = dir,
               .initial_quantity = qty,
               .remaining_quantity = qty,
               .time_ = std::chrono::system_clock::now(),
               .cur_level = nullptr,
               .next_order = nullptr,
               .prev_order = nullptr,
               .id = };
}

Message create_order_message(ID id, Price price, Quantity qty, Direction dir) {
  return Message{create_order(id, price, qty, dir)};
}
Message create_cancel_message(ID id) { return Message{Cancel{id}}; }

/**
TODO: For when we implement FOK orders
Order create_timed_order(ID id, Price price, Quantity qty, Direction dir,
                         std::chrono::milliseconds offset);
*/
