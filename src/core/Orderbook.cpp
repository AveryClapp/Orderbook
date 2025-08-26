#include "include/core/Orderbook.h"
#include "include/core/Message.h"
#include <algorithm>
#include <cassert>
#include <ctime>

Orderbook::Orderbook() : order_pool_(POOL_SIZE) {}

/**
 * Upon receiving an order, send it to a function that handles its direction.
 */
void Orderbook::receive_message(const Message &msg) {
  if (msg.type == MessageType::Order) [[likely]] {
    const NewOrderData &metadata = msg.data.new_order;
    if (metadata.type == OrderType::FillOrKill && !can_fill(metadata))
        [[unlikely]] {
      return; // Can't fill, exit here.
    }
    Order *order = order_pool_.get();
    *order = Order{.price = metadata.price,
                   .quantity = metadata.quantity,
                   .id = metadata.id,
                   .time = std::chrono::system_clock::now(),
                   .type = metadata.type,
                   .direction = metadata.direction};
    if (metadata.direction == Direction::Buy) {
      handle_buy(order);
    } else {
      handle_sell(order);
    }
  } else {
    const CancelData &cancel = msg.data.cancel;
    handle_cancel(cancel.order_id);
    return;
  }
}

void Orderbook::handle_sell(Order *sell_order) {
  const auto now = std::chrono::system_clock::now();
  const auto cutoff = now - std::chrono::hours(24);
  auto &bids = levels_.get_bids();
  auto it = bids.begin();
  while (sell_order->quantity > 0 && it != bids.end()) {
    auto &[price, level] = *it;
    if (price < sell_order->price) {
      break;
    }
    while (sell_order->quantity > 0) {
      if (level.orders.empty())
        break;
      Order *const order = level.orders.front();
      if (order->type == OrderType::GoodForDay && order->time <= cutoff) {
        level.orders.pop_front();
        order_map_.erase(order->id);
        order_pool_.release(order);
        continue;
      }
      Quantity consumed = std::min(order->quantity, sell_order->quantity);
      order->quantity -= consumed;
      sell_order->quantity -= consumed;
      level.total_quantity -= consumed;
      if (order->quantity == 0) {
        level.orders.pop_front();
        order_map_.erase(order->id);
        order_pool_.release(order);
      } // Otherwise, order is fulfilled and while loop is done
    }
    if (level.orders.empty()) {
      it = bids.erase(it);
    } else {
      ++it;
    }
  }
  if (sell_order->quantity > 0) {
    levels_.add_ask(sell_order);
    order_map_[sell_order->id] = sell_order;
  } else {
    order_pool_.release(sell_order);
  }
}

void Orderbook::handle_buy(Order *buy_order) {
  const auto now = std::chrono::system_clock::now();
  const auto cutoff = now - std::chrono::hours(24);
  auto &asks = levels_.get_asks();
  auto it = asks.begin();
  while (buy_order->quantity > 0 && it != asks.end()) {
    auto &[price, level] = *it;
    if (price > buy_order->price) {
      break;
    }
    while (buy_order->quantity > 0) {
      if (level.orders.empty())
        break;
      Order *const order = level.orders.front();
      if (order->type == OrderType::GoodForDay && order->time <= cutoff) {
        level.orders.pop_front();
        order_map_.erase(order->id);
        order_pool_.release(order);
        continue;
      }
      Quantity consumed = std::min(order->quantity, buy_order->quantity);
      order->quantity -= consumed;
      buy_order->quantity -= consumed;
      level.total_quantity -= consumed;
      if (order->quantity == 0) {
        level.orders.pop_front();
        order_map_.erase(order->id);
        order_pool_.release(order);
      }
    }
    if (level.orders.empty()) {
      it = asks.erase(it);
    } else {
      ++it;
    }
  }
  if (buy_order->quantity > 0) {
    levels_.add_bid(buy_order);
    order_map_[buy_order->id] = buy_order;
  } else {
    order_pool_.release(buy_order);
  }
}

void Orderbook::handle_cancel(const ID cancel_id) {
  assert(order_map_.find(cancel_id) != order_map_.end());
  Order *const order = order_map_[cancel_id];
  Level &target_level = (order->direction == Direction::Buy)
                            ? levels_.get_bids()[order->price]
                            : levels_.get_asks()[order->price];

  target_level.orders.erase(target_level.orders.begin() +
                            static_cast<std::ptrdiff_t>(order->level_position));

  for (size_t i = order->level_position; i < target_level.orders.size(); ++i) {
    target_level.orders[i]->level_position = i;
  }

  order_map_.erase(cancel_id);
  order_pool_.release(order);
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

bool Orderbook::can_fill(const NewOrderData &order_data) {
  Quantity matched_quantity = 0;
  const Quantity target_quantity = order_data.quantity;
  const Price order_price = order_data.price;
  if (order_data.direction == Direction::Sell) {
    const auto &bids = levels_.get_bids();
    for (const auto &[price, level] : bids) {
      if (price < order_price) {
        break;
      }
      matched_quantity += level.total_quantity;
      if (matched_quantity >= target_quantity) {
        return true;
      }
    }
  } else {
    const auto &asks = levels_.get_asks();
    for (const auto &[price, level] : asks) {
      if (price > order_price) {
        break;
      }
      matched_quantity += level.total_quantity;
      if (matched_quantity >= target_quantity) {
        return true;
      }
    }
  }
  return false;
}
