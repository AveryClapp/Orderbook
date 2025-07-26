#include "Order.h"

/* Set and get method to speed up matching process */
int8_t Order::reduceQuantity(Quantity delta) {
  if (delta > remaining_quanity_) {
    throw std::invalid_argument("Order has no more quantity");
  }
  remaining_quanity_ -= delta;
  return remaining_quantity_;
}

/* Encapsulate B-tree traversal here */
Order *Order::getNextOrder() const {
  /* A few cases to handle:
   * 1) Simple iterative movement
   * 2) If the level is complete, replace current level with the right subtree
   * 3) If no right subtree exists, then you go to the parent level
   */
  if (next_order_) {
    return next_order_;
  }
}

Order *Order::getPrevOrder() const {}
