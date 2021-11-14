#include <iostream>
#include "cache.hpp"
#include "my_array.hpp"
#include "function.hpp"
#include <iomanip>
#include <cassert>
#include <cmath>

const std::size_t size = 1024 * 1024; 
Cache cache;
my_array<size>& x(*(new my_array<size>(&cache)));
my_array<size>& y(*(new my_array<size>(&cache)));
my_array<size>& z(*(new my_array<size>(&cache)));
my_array<size>& xx(*(new my_array<size>(&cache)));
my_array<size>& yy(*(new my_array<size>(&cache)));
my_array<size>& zz(*(new my_array<size>(&cache)));

my_array<size>* RAM[6] = {&x, &y, &z, &xx, &yy, &zz};

void free_memory() {
    delete& x;
    delete& y;
    delete& z;
    delete& xx;
    delete& yy;
    delete& zz;
}

void fill_arrays(double value) {
    x.fill(value + 1.23);
    y.fill(value + 8901.23);
    z.fill(value);
    xx.fill(value * 2 + 123.321);
    yy.fill(value + 23.32);
    zz.fill(value * 10.12);
}

int main() {
    execute::f(2.3);

    std::cout << "L1: " << cache.stats.l1h << " hits from " << cache.stats.l1o << " asks -- " << std::setprecision(5) << (double) (cache.stats.l1h * 100.0 / cache.stats.l1o) << "%\n";
    std::cout << "L2: " << cache.stats.l2h << " hits from " << cache.stats.l2o << " asks -- " << std::setprecision(5) << (double) (cache.stats.l2h * 100.0 / cache.stats.l2o) << "%\n";

    free_memory();

    return 0;
}