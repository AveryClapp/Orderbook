#include "Orderbook.h"

/**
 * Upon receiving an order, send it to a function that handles its direction.
 */
void Orderbook::receive_message(Order *order) {
  /* TODO: Handle CANCEL orders */
  if (order->getDirection() == Direction::Buy) {
    handle_buy(order);
  } else if (order->getDirection() == Direction::Sell) {
    handle_sell(order);
  } else {
    throw std::runtime_error("Impossible Dud");
  }
}

void Orderbook::handle_buy(Order *buy_order) {
  Levels &asks = levels_.get_asks();
  for (Level *l : asks) {
    if (l->price > buy_order->price) {
      break;
    }
    if (l->num_orders == 0) {
      continue;
    }
    Order *cur = l->head;
    while (cur) {
      Quantity consumed =
          std::min(cur->remaining_quantity_, buy_order->remaining_quantity_);
      buy_order->remaining_quantity_ -= consumed;
      cur->remaining_quantity_ -= consumed;
      if (cur->remaining_quantity_ == 0) {
        // TODO: Fix this so that it actually updates the levels
        cur = cur->next;
        l->num_orders--;
      }
      if (buy_order->remaining_quantity_ == 0) {
        break;
      }
    }
  }
  if (buy_order->remaining_quantity_ > 0) {
    levels_.add_bid(buy_order);
  }
}

void Orderbook::handle_sell(Order *sell_order) {
  Levels &bids = levels_.get_bids();
  for (Level *l : bids) {
    if (l->price > sell_order->price) {
      break;
    }
    if (l->num_orders == 0) {
      continue;
    }
    Order *cur = l->head;
    while (cur) {
      Quantity consumed =
          std::min(cur->remaining_quantity_, sell_order->remaining_quantity_);
      sell_order->remaining_quantity_ -= consumed;
      cur->remaining_quantity_ -= consumed;
      if (cur->remaining_quantity_ == 0) {
        // TODO: Fix this so that it actually updates the levels
        cur = cur->next;
        l->num_orders--;
      }
      if (sell_order->remaining_quantity_ == 0) {
        break;
      }
    }
  }
  if (sell_order->remaining_quantity_ > 0) {
    levels_.add_ask(sell_order);
  }
}
