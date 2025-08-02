#include "include/core/Orderbook.h"
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
/**
 * Upon receiving an order, send it to a function that handles its direction.
 */
void Orderbook::receive_message(Message msg) {
  if (std::holds_alternative<Cancel>(msg.data)) {
    Cancel &cancel = std::get<Cancel>(msg.data);
    levels_.handle_cancel(cancel.cancel_id);
  } else if (std::holds_alternative<Order>(msg.data)) {
    Order &order = std::get<Order>(msg.data);
    if (order.direction == Direction::Buy) {
      levels_.handle_buy(&order);
    } else {
      levels_.handle_sell(&order);
    }
  } else {
    throw std::runtime_error("Impossible Dud");
  }
}

void Orderbook::print_book() {
  std::cout << "\n================== ORDER BOOK ==================\n";
  std::cout << std::setw(15) << "ASKS (SELL)" << std::setw(20) << ""
            << std::setw(15) << "BIDS (BUY)" << "\n";
  std::cout << std::setw(8) << "Price" << std::setw(8) << "Qty" << std::setw(8)
            << "Orders" << std::setw(8) << "|" << std::setw(8) << "Price"
            << std::setw(8) << "Qty" << std::setw(8) << "Orders" << "\n";
  std::cout << "------------------------------------------------\n";

  const auto &asks = levels_.get_asks();
  const auto &bids = levels_.get_bids();

  size_t max_levels = std::max(asks.size(), bids.size());
  size_t display_levels =
      std::min(max_levels, static_cast<size_t>(10)); // Show top 10 levels

  for (size_t i = 0; i < display_levels; ++i) {
    size_t ask_idx = asks.size() > i ? asks.size() - 1 - i : SIZE_MAX;
    if (ask_idx != SIZE_MAX && ask_idx < asks.size()) {
      Level *ask_level = asks[ask_idx];
      Quantity total_qty = 10;
      std::cout << std::setw(8) << ask_level->price << std::setw(8) << total_qty
                << std::setw(8) << ask_level->num_orders;
    } else {
      std::cout << std::setw(24) << "";
    }
  }
}
