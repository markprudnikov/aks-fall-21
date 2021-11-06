#include "cache.hpp"

template<int W>
void Cache::lru(Unit<W>& unit, int n) {
    for (; n > 0; n--)
        std::swap(unit[n], unit[n - 1]);
}

template<int W>
int Cache::find_in_unit(Unit<W>& unit, std::size_t head) {
    for (uint32_t i = 0; i < unit.size(); ++i)
        if ((std::size_t) unit[i].addrh == head && unit[i].addrh != -1)
            return i;

    return -1;
}

double& Cache::get(std::size_t addr) {
    stats.l1o += 1;

    std::size_t head = addr >> 3;
    std::size_t tail = addr & 0x7;

    Unit<4>& unit_l1 = L1[head % 512];
    //
    int pos = find_in_unit<4>(unit_l1, head);
    if (pos != -1) {
        stats.l1h += 1;

        lru<4>(unit_l1, pos);

        return unit_l1[0].cl_data[tail];
    }

    stats.l2o += 1;

    Unit<8>& unit_l2 = L2[head % 2048];
    pos = find_in_unit<8>(unit_l2, head);
    if (pos != -1) {
        stats.l2h += 1;

        lru<8>(unit_l2, pos);
        std::swap(unit_l2[0], unit_l1[3]);
        lru<4>(unit_l1, 3);

        return unit_l2[0].cl_data[tail];
    }

    

    // идём в оперативку (как?..)
    
    return ref; // <-- здесь бред, чтобы не было warning
}

void Cache::set(std::size_t addr, double val) {
    double& value = get(addr); // юзаем ли то, что ретернули?
    value = val;
    
    // return this?
}