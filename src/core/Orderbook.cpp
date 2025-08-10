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
                << std::setw(8) << ask_level->num_orders;
    } else {
      std::cout << std::setw(24) << "";
    }
  }
}

void Orderbook::handle_sell(Order *sell_order) {
  auto &bids = levels_.get_bids();
  for (Level *l : bids) {
    if (l->price < sell_order->price) {
      break;
    }
    if (l->num_orders == 0) {
      continue;
    }
    Order *cur = l->head;
    while (cur) {
      Quantity consumed =
          std::min(cur->remaining_quantity, sell_order->remaining_quantity);
      sell_order->remaining_quantity -= consumed;
      cur->remaining_quantity -= consumed;
      if (cur->remaining_quantity == 0) {
        Order *remove = cur;
        if (remove->prev_order) {
          remove->prev_order->next_order = remove->next_order;
        } else {
          l->head = remove->next_order;
        }
        if (remove->next_order) {
          remove->next_order->prev_order = remove->prev_order;
        }
        cur = remove->next_order;
        delete remove;
        l->num_orders--;
      }
      if (sell_order->remaining_quantity == 0) {
        break;
      }
    }
    if (sell_order->remaining_quantity == 0) {
      break;
    }
  }
  if (sell_order->remaining_quantity > 0) {
    levels_.add_ask(sell_order);
  }
}

void Orderbook::handle_buy(Order *buy_order) {
  auto &asks = levels_.get_asks();
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
          std::min(cur->remaining_quantity, buy_order->remaining_quantity);
      buy_order->remaining_quantity -= consumed;
      cur->remaining_quantity -= consumed;
      if (cur->remaining_quantity == 0) {
        Order *remove = cur;
        if (remove->prev_order) {
          remove->prev_order->next_order = remove->next_order;
        } else {
          l->head = remove->next_order;
        }
        if (remove->next_order) {
          remove->next_order->prev_order = remove->prev_order;
        }
        cur = remove->next_order;
        delete remove;
        l->num_orders--;
      }
      if (buy_order->remaining_quantity == 0) {
        break;
      }
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
  Order *remove = order;
  if (remove->prev_order) {
    remove->prev_order->next_order = remove->next_order;
  } else {
    remove->cur_level->head = remove->next_order;
  }
  if (remove->next_order) {
    remove->next_order->prev_order = remove->prev_order;
  }
  remove->cur_level->num_orders--;
  delete remove;
}

void Orderbook::get_best_bid() {
  auto &bids = levels_.get_bids();
  if (!bids.size()) {
    std::cout << "No active bids" << "\n";
  }
  return std::cout << "Best bid at price: " << bids[0].
}
