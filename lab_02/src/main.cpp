#include <iostream>
#include "cache.hpp"
#include "my_array.hpp"

const std::size_t size = 1024 * 1024;
my_array<size>& x = *(new my_array<size>(0));
my_array<size>& y = *(new my_array<size>(1));
my_array<size>& z = *(new my_array<size>(2));
my_array<size>& xx = *(new my_array<size>(3));
my_array<size>& yy = *(new my_array<size>(4));
my_array<size>& zz = *(new my_array<size>(5));

void free_memory() {
    delete& x;
    delete& y;
    delete& z;
    delete& xx;
    delete& yy;
    delete& zz;
}


int main() {

    Cache cache;

    x[0] = 5.00;
    x[1] = 6.00;
    x[0] = x[0] + 3.00 * x[1];
    std::cout << x[0] << '\n';

    std::cout << cache.stats.l1h << ' ' << cache.stats.l1o << ' ' << cache.stats.l1h << ' ' << cache.stats.l2o << '\n';

    free_memory();

    return 0;
}