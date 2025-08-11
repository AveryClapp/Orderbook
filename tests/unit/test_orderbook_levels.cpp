#include "include/core/OrderbookLevels.h"
#include "tests/helpers/test_utils.h"
#include <gtest/gtest.h>

TEST(TestOrderbookLevels, TestCreateEmpty) {
  OrderbookLevels levels = OrderbookLevels();
  EXPECT_TRUE(levels.get_bids().empty());
  EXPECT_TRUE(levels.get_asks().empty());
}

TEST(TestOrderbookLevels, TestAddAskLevel) {
  OrderbookLevels levels = OrderbookLevels();

  levels.add_ask(test_utils::create_order(1, 100, 100, Direction::Buy));
  EXPECT_FALSE(levels.get_asks().empty());
}

TEST(TestOrderbookLevels, TestAddAskWithExistingLevel) {
  OrderbookLevels levels = OrderbookLevels();
  levels.add_ask(test_utils::create_order(1, 100, 100, Direction::Buy));
  levels.add_ask(test_utils::create_order(2, 100, 150, Direction::Buy));
  EXPECT_FALSE(levels.get_asks().empty());
}

TEST(TestOrderbookLevels, ) {}
