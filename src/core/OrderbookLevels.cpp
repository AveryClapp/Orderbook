#include "include/core/OrderbookLevels.h"
#include <cassert>

void OrderbookLevels::add_ask(Order *ask) {
  // First check if we have this level already
  Level *target_level;
  auto it = ask_level_map_.find(ask->price);
  if (it != ask_level_map_.end()) {
    target_level = ask_level_map_[ask->price];
  } else {
    add_ask_level(ask);
  }
}

void OrderbookLevels::add_bid(Order *bid) {
  Level *target_level;
  auto it = bid_level_map_.find(bid->price);
  if (it != bid_level_map_.end()) {
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
  ask_level_map_[ask->price] = new_level;
  asks_.push_back(new_level);
}
