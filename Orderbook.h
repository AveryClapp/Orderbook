#pragma once

#include <algorithm>
#include <iostream>
#include <vector>

class OrderbookLevels {
    private:
        LevelInfos m_bids;
        LevelInfos m_asks;
    public:
        const int get_bids() const { return m_bids; }
        const int get_asks() const { return m_asks; }
}
