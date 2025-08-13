#include "include/core/Orderbook.h"
#include <algorithm>
#include <cassert>
#include <ctime>
#include <iomanip>
#include <iostream>

/**
 * Upon receiving an order, send it to a function that handles its direction.
 */
void Orderbook::receive_message(Message msg) {
  if (std::holds_alternative<Cancel>(msg.data)) {
    Cancel &cancel = std::get<Cancel>(msg.data);
    handle_cancel(cancel.cancel_id);
  } else if (std::holds_alternative<Order *>(msg.data)) {
    Order *order = std::get<Order *>(msg.data);
    if (order->type == OrderType::FillOrKill && !can_fill(*order)) {
      return; // Can't fill, exit here.
    }
    if (order->direction == Direction::Buy) {
      handle_buy(order);
    } else {
      handle_sell(order);
    }
  } else {
    throw std::runtime_error("Impossible Dud");
  }
}

// TODO: Fix
void Orderbook::print_book() {
  std::cout << "\n================== ORDER BOOK ==================\n";
  std::cout << std::setw(15) << "ASKS (SELL)" << std::setw(20) << ""
            << std::setw(15) << "BIDS (BUY)" << "\n";
  std::cout << std::setw(8) << "Price" << std::setw(8) << "Qty" << std::setw(8)
            << "Orders" << std::setw(8) << "|" << std::setw(8) << "Price"
            << std::setw(8) << "Qty" << std::setw(8) << "Orders" << "\n";
  std::cout << "------------------------------------------------\n";

  auto &asks = levels_.get_asks();
  auto &bids = levels_.get_bids();

  size_t max_levels = std::max(asks.size(), bids.size());
  size_t display_levels = std::min(max_levels, static_cast<size_t>(10));

  for (size_t i = 0; i < display_levels; ++i) {
    size_t ask_idx = asks.size() > i ? asks.size() - 1 - i : SIZE_MAX;
    if (ask_idx != SIZE_MAX && ask_idx < asks.size()) {
      Level ask_level = asks[ask_idx];
      Quantity total_qty = 10;
      std::cout << std::setw(8) << ask_level.price << std::setw(8) << total_qty
                << std::setw(8) << ask_level.orders.size();
    } else {
      std::cout << std::setw(24) << "";
    }
  }
}

void Orderbook::handle_sell(Order *sell_order) {
  auto &bids = levels_.get_bids();
  auto it = bids.begin();
  while (it != bids.end() && sell_order->remaining_quantity > 0) {
    auto &[price, level] = *it;
    if (price < sell_order->price) {
      break;
    }
    unsigned short orders_filled = 0;
    auto order_it = level.orders.begin();
    while (order_it != level.orders.end()) {
      auto *order = *order_it;
      Quantity consumed =
          std::min(order->remaining_quantity, sell_order->remaining_quantity);
      order->remaining_quantity -= consumed;
      sell_order->remaining_quantity -= consumed;
      level.total_quantity -= consumed;
      if (order->remaining_quantity == 0) {
        order_it = level.orders.erase(order_it);
        order_map_.erase(sell_order->id);
        delete sell_order;
      } else {
        ++order_it;
      }
    }
    if (level.orders.empty()) {
      it = bids.erase(it);
    } else {
      ++it;
    }
  }
  if (sell_order->remaining_quantity > 0) {
    levels_.add_ask(sell_order);
  }
}

void Orderbook::handle_buy(Order *buy_order) {
  auto &asks = levels_.get_asks();
  auto it = asks.begin();
  while (it != asks.end() && buy_order->remaining_quantity > 0) {
    auto &[price, level] = *it;
    if (price > buy_order->price) {
      break;
    }
    unsigned short orders_filled = 0;
    auto order_it = level.orders.begin();
    while (order_it != level.orders.end()) {
      auto *order = *order_it;
      Quantity consumed =
          std::min(order->remaining_quantity, buy_order->remaining_quantity);
      order->remaining_quantity -= consumed;
      buy_order->remaining_quantity -= consumed;
      level.total_quantity -= consumed;
      if (order->remaining_quantity == 0) {
        order_it = level.orders.erase(order_it);
        order_map_.erase(buy_order->id);
        delete buy_order;
      } else {
        ++order_it;
      }
    }
    if (level.orders.empty()) {
      it = asks.erase(it);
    } else {
      ++it;
    }
  }
  if (buy_order->remaining_quantity > 0) {
    levels_.add_bid(buy_order);
  }
}

void Orderbook::handle_cancel(const ID cancel_id) {
  Order *order = order_map_[cancel_id];
  if (!order) {
    throw std::runtime_error("Tried to Cancel nonexistent id");
  }
  assert(order->id == cancel_id);
  Level &target_level = (order->direction == Direction::Buy)
                            ? levels_.get_bids()[order->price]
                            : levels_.get_asks()[order->price];
  target_level.cancel_order(order->level_position);

  order_map_.erase(cancel_id);
  delete order;
}

std::optional<std::pair<Price, size_t>> Orderbook::get_best_bid() {
  auto &bids = levels_.get_bids();
  if (!bids.size()) {
    return std::nullopt;
  }
  const auto &best = *bids.begin();
  return std::make_pair(best.first, best.second.orders.size());
}

std::optional<std::pair<Price, size_t>> Orderbook::get_best_ask() {
  auto &asks = levels_.get_asks();
  if (!asks.size()) {
    return std::nullopt;
  }
  const auto &best = *asks.begin();
  return std::make_pair(best.first, best.second.orders.size());
}

bool Orderbook::can_fill(const Order &order) {
  Quantity matched_quantity = 0;
  if (order.direction == Direction::Sell) {
    auto &bids = levels_.get_bids();
    for (const auto &[price, level] : bids) {
      if (price < order.price || matched_quantity > order.initial_quantity) {
        break;
      }
      matched_quantity += level.total_quantity;
    }
  } else {
    auto &asks = levels_.get_asks();
    for (const auto &[price, level] : asks) {
      if (price > order.price || matched_quantity > order.initial_quantity) {
        break;
      }
      matched_quantity += level.total_quantity;
    }
  }
  return matched_quantity >= order.initial_quantity;
}
