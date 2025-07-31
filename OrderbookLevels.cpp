#include "OrderbookLevels.h"
#include <cassert>

void OrderbookLevels::add_ask(Order *ask) {
  // First check if we have this level already
  Level *target_level;
  auto it = level_map_.find(ask->price);
  if (it != level_map_.end()) {
    target_level = it->second;
  } else {
    add_ask_level(ask);
  }
}

void OrderbookLevels::add_bid(Order *bid) {
  Level *target_level;
  auto it = level_map_.find(bid->price);
  if (it != level_map_.end()) {
    target_level = it->second;
  } else {
    add_bid_level(bid);
  }
}

void OrderbookLevels::add_bid_level(Order *bid) {
  Level *new_level = new Level{bid->price, 1, bid, bid};
  for (size_t i = 0; i < bids_.size(); i++) {
    if (bid->price < bids_[i]->price) {
      bids_.emplace(bids_.begin() + i, new_level);
      bid_level_map_[bid->price] = new_level;
      return;
    }
  }
  bid_level_map_[bid->price] = new_level;
  bids_.push_back(new_level);
}

void OrderbookLevels::add_ask_level(Order *ask) {
  Level *new_level = new Level{ask->price, 1, ask, ask};
  for (size_t i = 0; i < asks_.size(); i++) {
    if (ask->price < asks_[i]->price) {
      asks_.emplace(asks_.begin() + i, new_level);
      ask_level_map_[ask->price] = new_level;
      return;
    }
  }
  ask_level_map_[bid->price] = new_level;
  asks_.push_back(new_level);
}

void OrderbookLevels::handle_sell(Order *sell_order) {
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
        Level *remove = cur;
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
    add_ask(sell_order);
  }

  void Orderbook::handle_buy(Order * buy_order) {
    for (Level *l : asks_) {
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
        buy_order->remaining_quantity_ -= consumed;
        cur->remaining_quantity -= consumed;
        if (cur->remaining_quantity_ == 0) {
          Level *remove = cur;
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
      add_bid(buy_order);
    }
  }
}

void OrderbookLevels::cancel_order(const Cancel cancel_id) {
  Order *order = order_map[cancel_id];
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
