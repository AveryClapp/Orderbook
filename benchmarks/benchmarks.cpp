#include "include/core/Orderbook.h"
#include "tests/helpers/test_utils.h"
#include <benchmark/benchmark.h>
#include <random>
#include <vector>

static void BM_PureInsertionThroughput(benchmark::State &state) {
  const int64_t num_orders = state.range(0);

  for (auto _ : state) {
    state.PauseTiming();

    Orderbook orderbook;
    std::vector<Message> orders;
    orders.reserve(num_orders);

    std::mt19937 gen(42);
    std::uniform_int_distribution<Price> price_dist(9000, 11000);
    std::uniform_int_distribution<Quantity> qty_dist(1, 100);
    std::bernoulli_distribution side_dist(0.5);

    for (int64_t i = 0; i < num_orders; ++i) {
      Direction dir = side_dist(gen) ? Direction::Buy : Direction::Sell;
      Price price = price_dist(gen);

      if (dir == Direction::Buy) {
        price = std::min(price, Price(9500));
      } else {
        price = std::max(price, Price(10500));
      }

      NewOrderData order = test_utils::create_order(static_cast<ID>(i), price,
                                                    qty_dist(gen), dir);
      orders.emplace_back(Message{MessageType::Order, {.new_order = order}});
    }

    state.ResumeTiming();

    for (const auto &msg : orders) {
      orderbook.receive_message(msg);
    }

    state.counters["OrdersPerSecond"] = benchmark::Counter(
        static_cast<double>(num_orders), benchmark::Counter::kIsRate);
    state.counters["OrdersProcessed"] = static_cast<double>(num_orders);
  }
}
BENCHMARK(BM_PureInsertionThroughput)
    ->Range(1000, 100000)
    ->Unit(benchmark::kMicrosecond)
    ->Iterations(10);

BENCHMARK_MAIN();
