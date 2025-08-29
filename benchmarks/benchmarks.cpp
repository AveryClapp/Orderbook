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
  for (auto _ : state) {
    state.PauseTiming();
    OrderbookFixture fixture;
    state.ResumeTiming();

    NewOrderData order = test_utils::create_order(1, 100, 10, Direction::Buy);
    Message msg{MessageType::Order, {.new_order = order}};
    fixture.orderbook.receive_message(msg);
    benchmark::DoNotOptimize(fixture.orderbook);
  }
}
BENCHMARK(BM_SingleOrderLatency);

BENCHMARK_MAIN();
