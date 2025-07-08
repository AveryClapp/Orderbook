#include <algorithm>
#include <iostream>


enum class Direction {
    buy,
    sell,
};

enum class OrderType {
    FillOrKill,
    GoodTilCancel,
};

using Price = int32_t;
using Quantity = uint32_t;

int main() {
    Price x = 32;
    std::cout << x << std::endl;

}
