#pragma once
#include <array>
#include <cstddef>
#include <algorithm>
#include <cstdint>

struct Stats {
    std::size_t l1h = 0;
    std::size_t l1o = 0;
    
    std::size_t l2h = 0;
    std::size_t l2o = 0;
};

struct CacheLine {
    bool modified = false;
    int64_t addrh = -1;
    std::array<double, 8> cl_data;
};

template<int W>
using Unit = std::array<CacheLine, W>;

struct Cache {
    std::array<Unit<4>, 512> L1;
    std::array<Unit<8>, 2048> L2;
    Stats stats;


    template<int W>
    static void lru(Unit<W>& unit, int n);

    template<int W>
    static int find_in_unit(Unit<W>& unit, std::size_t head);

    void modify(int64_t addrh, CacheLine new_cl);

};