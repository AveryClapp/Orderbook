# Orderbook

## Overview

This order matching engine is designed for high-frequency trading environments where microsecond latency matters. It implements multiple matching algorithms and supports various order types commonly used in modern electronic exchanges. Currently, this is just for Limit order books.

## Iterative Benchmarking

With the goal of making this as fast as possible, I figured it would be a good idea to document the journey from making this fast as possible starting from a working version. This starts with no explicit optimizations and then goes through step by step what I refined to improve performance.

Some Specs:

- L1 Data Cache: 64 KiB
- L1 Instruction Cache: 124 KiB
- L2 Data Cache: 4096 KiB
- 8 x 2400 MHz CPUs (more on this later)

### Iteration 1
