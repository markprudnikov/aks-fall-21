#pragma once
#include <cstddef>
#include "cache.hpp"

template<std::size_t N>
class my_array {
private:
    static const std::size_t _size = (N + 8 - 1) / 8;
    Cache* cache;
public:
    CacheLine _data[_size];

    my_array() {
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
        // std::size_t addrh = _data[index / 8].addrh;
        // cache->get(addrh, index % 8);
        return _data[index / 8].cl_data[index % 8];
    }

    double_ref operator[](std::size_t index) { 
        return double_ref(_data, index);
    }

    void fill(double value) {
        for (std::size_t index = 0; index < N; ++index)
            (*this)[index] = value;
    }

};
