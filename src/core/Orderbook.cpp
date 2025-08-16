#include "include/core/Orderbook.h"
#include <algorithm>
#include <cassert>
#include <ctime>

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

void Orderbook::handle_sell(Order *sell_order) {
  auto now = std::chrono::system_clock::now();
  auto &bids = levels_.get_bids();
  auto it = bids.begin();
  while (it != bids.end() && sell_order->remaining_quantity > 0) {
    auto &[price, level] = *it;
    if (price < sell_order->price) {
      break;
    }
    auto order_it = level.orders.begin();
    while (order_it != level.orders.end()) {
      auto *order = *order_it;
      if (now - order->time >= std::chrono::hours(24) &&
          order->type == OrderType::GoodForDay) {
        order_it = level.orders.erase(order_it);
        delete order;
        continue;
      }
      Quantity consumed =
          std::min(order->remaining_quantity, sell_order->remaining_quantity);
      order->remaining_quantity -= consumed;
      sell_order->remaining_quantity -= consumed;
      level.total_quantity -= consumed;
      if (order->remaining_quantity == 0) {
        order_it = level.orders.erase(order_it);
        order_map_.erase(order->id);
        delete order;
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
    order_map_[sell_order->id] = sell_order;
  } else {
    delete sell_order;
  }
}

void Orderbook::handle_buy(Order *buy_order) {
  auto now = std::chrono::system_clock::now();
  auto &asks = levels_.get_asks();
  auto it = asks.begin();
  while (it != asks.end() && buy_order->remaining_quantity > 0) {
    auto &[price, level] = *it;
    if (price > buy_order->price) {
      break;
    }
    auto order_it = level.orders.begin();
    while (order_it != level.orders.end()) {
      auto *order = *order_it;
      if (now - order->time >= std::chrono::hours(24) &&
          order->type == OrderType::GoodForDay) {
        order_it = level.orders.erase(order_it);
        delete order;
        continue;
      }
      Quantity consumed =
          std::min(order->remaining_quantity, buy_order->remaining_quantity);
      order->remaining_quantity -= consumed;
      buy_order->remaining_quantity -= consumed;
      level.total_quantity -= consumed;
      if (order->remaining_quantity == 0) {
        order_it = level.orders.erase(order_it);
        order_map_.erase(order->id);
        delete order;
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
    order_map_[buy_order->id] = buy_order;
  } else {
    delete buy_order;
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
