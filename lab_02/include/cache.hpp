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

struct CacheLine { // cache->read(_data[i].addrh)
    bool modified = false;
    std::size_t addrh;
    std::array<double, 8> cl_data;
};

template<int W>
using Unit = std::array<CacheLine, W>;

struct Cache {
    std::array<Unit<4>, 512> L1;
    std::array<Unit<8>, 2048> L2;
    double ref = 5.00; // фиктивный элемент для get
    Stats stats;


    template<int W>
    static void lru(Unit<W>& unit, int n);

    template<int W>
    static int find_in_unit(Unit<W>& unit, std::size_t head);

    double& get(std::size_t addr);

    void set(std::size_t addr, double val);
};