#pragma once
#include <cstddef>
#include "cache.hpp"
#include <iostream>

template<std::size_t N>
class my_array {
private:
    static const std::size_t _size = (N + 8 - 1) / 8;
    Cache* cache;
    CacheLine _data[_size];
public:

    my_array(Cache* cp) : cache(cp) {
        static std::size_t k = 0;

        for (uint64_t i = 0; i < _size; ++i)
            _data[i].addrh = k * _size + i;
        
        k += 1;
    }

    class double_ref {
    public:
        double_ref(CacheLine* p, std::size_t index) :
            data_index(index >> 3),
            value_index(index & 0x7),
            data(p)
            {};

        operator double() const {
            return (double)(data[data_index].cl_data[value_index]);
        }
        
        double_ref& operator=(double new_value) {
            data[data_index].cl_data[value_index] = new_value;
            return *this;
        }

        double_ref& operator=(double_ref const& new_value) {
            if (this == &new_value)
                return *this;

            *this = (double) new_value;
            return *this;
        }

    private:
        std::size_t data_index;
        uint8_t value_index;
        CacheLine* data;
    };

    double operator[](std::size_t index) const {
        int64_t addrh = _data[index / 8].addrh;
        cache->modify(addrh, _data[index / 8]);

        return _data[index / 8].cl_data[index % 8];
    }

    double_ref operator[](std::size_t index) { 
        int64_t addrh = _data[index / 8].addrh;
        cache->modify(addrh, _data[index / 8]);

        return double_ref(_data, index);
    }

    void fill(double value) {
        for (std::size_t index = 0; index < N; ++index)
            _data[index / 8].cl_data[index % 8] = value;
    }

    std::size_t size() {
        return _size;
    }

    void set_cl(std::size_t index, CacheLine const& new_cl) {
        _data[index] = new_cl;
    }

};
