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
  if (next_order_) { // Return the next order in the level if it exists
    cur_level_.tail = next_order_;
    return next_order_;
  } else if (!right_child) { // Return the first element of the parent
    return cur_level_.parent.tail;
  } else { // If the right child exists, move that up in place of the current
    cur_level_.parent.left_child = cur_level_.right_child;
    cur_level_.right_child.parent = cur_level_.parent;
    return cur_level_.right_child.tail;
  }
}

Order *Order::getPrevOrder() const {}
