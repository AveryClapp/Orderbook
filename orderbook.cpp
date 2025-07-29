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
  Price buy_price = buy_order->getPrice();
  for (int i = 0; i < asks.size(); i++) {
  }

  if (buy_order->getRemainingQuantity() > 0) {
    levels_.add_bid(buy_order);
  }
}

void Orderbook::handle_sell(Order *sell_order) {}
