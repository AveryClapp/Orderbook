#include "Orderbook.h"

/**
 * Upon receiving an order, send it to a function that handles its direction.
 */
void Orderbook::receive_message(Message msg) {
  if (std::holds_alternative<Cancel>(msg.data)) {
    levels_handle_cancel(std::get<Cancel>msg.data));
  } else if (std::holds_alternative<Order>(msg.data)) {
    Order &order = std::get<Order>(msg_data);
    if (order->direction == Direction::Buy) {
      levels_.handle_buy(&order);
    } else {
      levels_.handle_sell(&order);
    }
  } else {
    throw std::runtime_error("Impossible Dud");
  }
}
