#include "cache.hpp"
#include "my_array.hpp"

const std::size_t size = 16;
extern my_array<size>& x;
extern my_array<size>* RAM[6];

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

void Cache::get(int64_t addrh, CacheLine new_cl) {
    stats.l1o += 1;

    // std::size_t head = addr >> 3;
    // std::size_t tail = addr & 0x7;

    Unit<4>& unit_l1 = L1[addrh % 512];
    
    int pos = find_in_unit<4>(unit_l1, addrh);
    if (pos != -1) {
        stats.l1h += 1;

        lru<4>(unit_l1, pos);

        // return unit_l1[0].cl_data[addrt];
        return;
    }

    stats.l2o += 1;

    Unit<8>& unit_l2 = L2[addrh % 2048];
    pos = find_in_unit<8>(unit_l2, addrh);
    if (pos != -1) {
        stats.l2h += 1;

        lru<8>(unit_l2, pos);
        std::swap(unit_l2[0], unit_l1[3]);
        lru<4>(unit_l1, 3);

        // return unit_l2[0].cl_data[addrt];
        return;
    }

    // в кеше ничего не оказалось
    // берём линию из оперативки и хуячим в L1, вытесненное из L1 в L2, вытесненное из L2 в оперативку

    std::swap(new_cl, unit_l1[3]);
    // в new_cl вытесненное из L1
    lru<4>(unit_l1, 3);

    std::swap(new_cl, unit_l2[7]);
    lru<8>(unit_l2, 7);

    // в new_cl вытесненное из L2
    if (!new_cl.modified)
        return;
        
    new_cl.modified = 0;

    uint8_t k = new_cl.addrh / x.size();

    std::size_t i = new_cl.addrh % x.size();

    RAM[k][i].set_cl(i, new_cl);
}

void Cache::set(std::size_t addr, double val) {
    // double& value = get(addr); // юзаем ли то, что ретернули?
    // value = val;
    (void) addr;
    (void) val;
    // return this?
}