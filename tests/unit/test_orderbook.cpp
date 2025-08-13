#include "include/core/Orderbook.h"
#include "tests/helpers/test_utils.h"
#include <gtest/gtest.h>

class OrderbookCoreTest : public ::testing::Test {
protected:
  void SetUp() override { orderbook = std::make_unique<Orderbook>(); }

  std::unique_ptr<Orderbook> orderbook;
};

TEST_F(OrderbookCoreTest, TestAddSingleBuyOrder) {
  Message buy_msg =
      test_utils::create_order_message(1, 100, 50, Direction::Buy);

  orderbook->receive_message(buy_msg);
  auto result = orderbook->get_best_bid();
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ((std::pair{100, 1}), result.value());

  ASSERT_FALSE(orderbook->get_best_ask().has_value());
}

TEST_F(OrderbookCoreTest, TestAddSingleSellOrder) {
  Message sell_msg =
      test_utils::create_order_message(1, 105, 30, Direction::Sell);

  orderbook->receive_message(sell_msg);

  auto ask_result = orderbook->get_best_ask();
  ASSERT_TRUE(ask_result.has_value());
  EXPECT_EQ((std::pair{105, 1}), ask_result.value());

  ASSERT_FALSE(orderbook->get_best_bid().has_value());
}

TEST_F(OrderbookCoreTest, TestMultipleBuyOrdersSamePrice) {
  Message buy1 = test_utils::create_order_message(1, 100, 25, Direction::Buy);
  Message buy2 = test_utils::create_order_message(2, 100, 35, Direction::Buy);

  orderbook->receive_message(buy1);
  orderbook->receive_message(buy2);

  auto bid_result = orderbook->get_best_bid();
  ASSERT_TRUE(bid_result.has_value());
  EXPECT_EQ((std::pair{100, 2}), bid_result.value());
}

TEST_F(OrderbookCoreTest, TestMultipleBuyOrdersDifferentPrices) {
  Message buy_high =
      test_utils::create_order_message(1, 102, 25, Direction::Buy);
  Message buy_low = test_utils::create_order_message(2, 98, 35, Direction::Buy);

  orderbook->receive_message(buy_high);
  orderbook->receive_message(buy_low);

  auto bid_result = orderbook->get_best_bid();
  ASSERT_TRUE(bid_result.has_value());
  EXPECT_EQ((std::pair{102, 1}), bid_result.value());
}

TEST_F(OrderbookCoreTest, TestMultipleSellOrdersDifferentPrices) {
  Message sell_low =
      test_utils::create_order_message(1, 105, 25, Direction::Sell);
  Message sell_high =
      test_utils::create_order_message(2, 110, 35, Direction::Sell);

  orderbook->receive_message(sell_low);
  orderbook->receive_message(sell_high);

  auto ask_result = orderbook->get_best_ask();
  ASSERT_TRUE(ask_result.has_value());
  EXPECT_EQ((std::pair{105, 1}), ask_result.value());
}

TEST_F(OrderbookCoreTest, TestExactMatch) {
  Message sell_msg =
      test_utils::create_order_message(1, 100, 50, Direction::Sell);
  orderbook->receive_message(sell_msg);

  Message buy_msg =
      test_utils::create_order_message(2, 100, 50, Direction::Buy);
  orderbook->receive_message(buy_msg);

  ASSERT_FALSE(orderbook->get_best_bid().has_value());
  ASSERT_FALSE(orderbook->get_best_ask().has_value());
}

TEST_F(OrderbookCoreTest, TestPartialFillBuyAggressive) {
  Message sell_msg =
      test_utils::create_order_message(1, 100, 100, Direction::Sell);
  orderbook->receive_message(sell_msg);

  Message buy_msg =
      test_utils::create_order_message(2, 100, 30, Direction::Buy);
  orderbook->receive_message(buy_msg);

  auto ask_result = orderbook->get_best_ask();
  ASSERT_TRUE(ask_result.has_value());
  EXPECT_EQ((std::pair{100, 1}), ask_result.value());

  ASSERT_FALSE(orderbook->get_best_bid().has_value());
}

TEST_F(OrderbookCoreTest, TestPartialFillSellAggressive) {
  Message buy_msg =
      test_utils::create_order_message(1, 100, 100, Direction::Buy);
  orderbook->receive_message(buy_msg);

  Message sell_msg =
      test_utils::create_order_message(2, 100, 40, Direction::Sell);
  orderbook->receive_message(sell_msg);

  auto bid_result = orderbook->get_best_bid();
  ASSERT_TRUE(bid_result.has_value());
  EXPECT_EQ((std::pair{100, 1}), bid_result.value());

  ASSERT_FALSE(orderbook->get_best_ask().has_value());
}

TEST_F(OrderbookCoreTest, TestMultipleLevelConsumption) {
  Message sell1 = test_utils::create_order_message(1, 100, 25, Direction::Sell);
  Message sell2 = test_utils::create_order_message(2, 101, 30, Direction::Sell);
  Message sell3 = test_utils::create_order_message(3, 102, 20, Direction::Sell);

  orderbook->receive_message(sell1);
  orderbook->receive_message(sell2);
  orderbook->receive_message(sell3);

  auto ask_result = orderbook->get_best_ask();
  ASSERT_TRUE(ask_result.has_value());
  EXPECT_EQ((std::pair{100, 1}), ask_result.value());

  ASSERT_EQ(std::nullopt, orderbook->get_best_bid());
}

TEST_F(OrderbookCoreTest, TestPriceImprovement) {
  Message sell_msg =
      test_utils::create_order_message(1, 105, 50, Direction::Sell);
  orderbook->receive_message(sell_msg);

  Message buy_msg =
      test_utils::create_order_message(2, 110, 25, Direction::Buy);
  orderbook->receive_message(buy_msg);

  auto ask_result = orderbook->get_best_ask();
  ASSERT_TRUE(ask_result.has_value());
  EXPECT_EQ((std::pair{105, 1}), ask_result.value());

  // Bid quantity should be completely matched, no leftover bid
  EXPECT_EQ(std::nullopt, orderbook->get_best_bid());
  orderbook->print_book();
}
