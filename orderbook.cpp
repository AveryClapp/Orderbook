#include "Orderbook.h"

/**
 * Upon receiving an order, send it to a function that handles its direction.
 */
void Orderbook::receive_order(const Order &order) {
  if (order.direction == Direction::Buy) {
    handle_buy(order);
  } else if (order.direction == Direction::Sell) {
    handle_sell(order);
  } else {
    throw std::runtime_error("Impossible Dud");
  }
}

/**
 * Upon receiving a buy, we maych all asks that fulfill the condition
 * and then add the buy to the bids
 */
void Orderbook::handle_buy(const Order &buy_order) {
  const auto &asks = levels_.get_asks();
  std::vector<std::pair<price, quantity>> updates{};
  for (auto &level : asks) {
    Price level_price = level.first;
    Quantity remaining_quantity = buy_order.getRemainingQuantity();

    if (level_price <= buy_order) {
      Quantity max_quantity = std::min(remaining_quantity, level.second);
      // Case 1: The order is fulfilled
      buy_order.remaining_quantity -= max_quantity;
      updates.emplace_back({level_price, level.second - max_quantity)}
      if (buy_order.remaining_quantity == 0) {
        break;
      }
    } else {
      break; // No other orders will fulfill our requirements
    }
  }
  // Second pass for updating levelinfos
  for (const auto &update : updates) {
    if (update.second == 0) {
      asks.erase(update.first);
    } else {
      asks[update.first] = update.second;
    }
  }

  if (buy_order.getRemainingQuantity() > 0) {
    add_bid(buy_order)
  }
}

void Orderbook::handle_sell(const Order &sell_order) {
  const auto &bids = levels_.get_bids();
  std::vector<std::pair<price, quantity>> updates{};
  for (const auto &level : bids) {
    Price level_price = level.first;
    Quantity remaining_quantity = sell_order.getRemainingQuantity();

    if (level_price >= sell_order) {
      Quantity max_quantity = std::min(remaining_quantity, level.second);

      sell_order.remaining_quantity -= max_quantity;
      updates.emplace_back({level_price, level.second - max_quantity)}
      if (sell_order.remaining_quantity == 0) {
        break;
      }
    } else {
      break; // No other orders will fulfill our requirements
    }
  }

  // Second pass for updating levelinfos
  for (const auto &update : updates) {
    if (update.second == 0) {
      bids.erase(update.first);
    } else {
      bids[update.first] = update.second;
    }
  }
  if (sell_order.getRemainingQuantity() > 0) {
    add_ask(sell_order)
  }
}
