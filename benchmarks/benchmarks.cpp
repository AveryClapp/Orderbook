#include "include/core/Orderbook.h"
#include "tests/helpers/test_utils.h"
#include <benchmark/benchmark.h>
#include <random>

class OrderbookFixture {
public:
  Orderbook orderbook;
  std::mt19937 gen{42};
};

static void BM_SingleOrderLatency(benchmark::State &state) {
  OrderbookFixture fixture;

  ID order_id = 1;
  Price start_price = 1;
  for (auto _ : state) {
    NewOrderData order = test_utils::create_order(order_id++, start_price + 1,
                                                  10, Direction::Buy);
    Message msg{MessageType::Order, {.new_order = order}};

    fixture.orderbook.receive_message(msg);
    benchmark::DoNotOptimize(fixture.orderbook);
  }
}
BENCHMARK(BM_SingleOrderLatency);

BENCHMARK_MAIN();
