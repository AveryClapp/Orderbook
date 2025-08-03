#pragma once

#include "include/core/Message.h"
#include "include/core/Order.h"
#include "include/core/Orderbook.h"

class TestOrderFactory {
public:
  static Order create_order(ID id, Price price, Quantity qty, Direction dir);
  static Message create_order_message(ID id, Price price, Quantity qty,
                                      Direction dir);
  static Message create_cancel_message(ID id);

  static Order create_timed_order(ID id, Price price, Quantity qty,
                                  Direction dir,
                                  std::chrono::milliseconds offset);
};

void verify_level_price_ordering(const Levels &bids, const Levels &asks);
void verify_fifo_within_level(const Level *level);
void verify_no_empty_levels(const Levels &levels);
void verify_order_map_consistency(const Orderbook &book);

std::vector<Order> create_ask_ladder(Price start_price, size_t num_levels);
std::vector<Order> create_bid_ladder(Price start_price, size_t num_levels);

#define EXPECT_ORDER_FILLED(order) EXPECT_EQ(order.remaining_quantity, 0)
#define EXPECT_ORDER_UNFILLED(order)                                           \
  EXPECT_EQ(order.remaining_quantity, order.initial_quantity)
