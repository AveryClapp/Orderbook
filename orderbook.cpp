#include "Orderbook.h"

/**
 * Upon receiving an order, send it to a function that handles its direction.
 */
void Orderbook::receive_order(const Order &order) {
  if (order.direction == Direction::Buy) {
    Orderbook::handle_buy(order);
  } else if (direction == Direction::Sell) {
    Orderbook::handle_sell(order);
  } else {
    throw std::runtime_error("Impossible Dud");
  }
}

/**
 * Return true on success
 */
bool Orderbook::handle_buy(const Order &buy_order) {
  const LevelInfos &asks = levels_.get_asks();

  for (LevelInfo &level : asks) {
  }
}
