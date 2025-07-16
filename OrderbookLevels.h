#pragma once

#include "LevelInfo.h"
#include "Using.h"


class OrderbookLevels {
    private:
        LevelInfos m_bids;
        LevelInfos m_asks;
    public:
        const LevelInfos& get_bids() const { return m_bids; }
        const LevelInfos& get_asks() const { return m_asks; }
};
