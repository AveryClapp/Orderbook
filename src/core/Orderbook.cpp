#include "include/core/Orderbook.h"
#include <algorithm>
#include <cassert>
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
    handle_cancel(cancel.cancel_id);
  } else if (std::holds_alternative<Order *>(msg.data)) {
    Order *order = std::get<Order *>(msg.data);
    if (order->direction == Direction::Buy) {
      handle_buy(order);
    } else {
      handle_sell(order);
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
                << std::setw(8) << ask_level->orders.size();
    } else {
      std::cout << std::setw(24) << "";
    }
  }
}

void Orderbook::handle_sell(Order *sell_order) {
  // Get the current bids
  auto &bids = levels_.get_bids();

  for (auto &[price, level] : bids) {
    if (price < sell_order->price) {
      break;
    }
    unsigned short orders_filled = 0;
    for (auto &order : level.orders) {
      Quantity consumed =
          std::min(order->remaining_quantity, sell_order->remaining_quantity);
      order->remaining_quantuty -= consumed;
      sell_order->remaining_quantity -= consumed;
      if (order->remaining_quantity == 0) {
        orders_filled++;
      }
      if (sell_order->remaining_quantity == 0) {
        break;
      }
    }
    for (unsigned j = 0; j < orders_filled; j++) {
      level.orders.pop_front();
    }
  }
  if (sell_order->remaining_quantity > 0) {
    levels_.add_ask(sell_order);
  }
}

void Orderbook::handle_buy(Order *buy_order) {
  auto &asks = levels_.get_asks();

  for (auto &[price, level] : asks) {
    if (price < buy_order->price) {
      break;
    }
    unsigned short orders_filled = 0;
    for (auto &order : level.orders) {
      Quantity consumed =
          std::min(order->remaining_quantity, sell_order->remaining_quantity);
      order->remaining_quantuty -= consumed;
      buy_order->remaining_quantity -= consumed;
      if (order->remaining_quantity == 0) {
        orders_filled++;
      }
      if (buy_order->remaining_quantity == 0) {
        break;
      }
    }
    for (unsigned j = 0; j < orders_filled; j++) {
      level.orders.pop_front();
    }
  }
  if (buy_order->remaining_quantity > 0) {
    levels_.add_ask(buy_order);
  }
}

void Orderbook::handle_cancel(const ID cancel_id) {
  Order *order = order_map_[cancel_id];
  if (!order) {
    throw std::runtime_error("Tried to Cancel nonexistent id");
  }
  assert(order->id == cancel_id);
  auto &orders = order->cur_level->orders;
  orders.erase(orders.begion() + order->level_position);
  delete order;
}

void Orderbook::get_best_bid() {
  auto &bids = levels_.get_bids();
  if (!bids.size()) {
    std::cout << "No active bids" << "\n";
    return;
  }
  const auto &smallestElement = *bids.begin();
  std::cout << "Best bid at price: " << smallestElement.price
            << " w/ quantity: " << smallestElement.num_orders << "\n";
}

void Orderbook::get_best_ask() {
  auto &asks = levels_.get_asks();
  if (!asks.size()) {
    std::cout << "No active bids" << "\n";
    return;
  }
  const auto &smallestElement = *asks.begin();
  std::cout << "Best bid at price: " << smallestElement.price
            << " w/ quantity: " << smallestElement.num_orders << "\n";
}
