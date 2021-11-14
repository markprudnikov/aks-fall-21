#include "function.hpp"
#include "cache.hpp"
#include "my_array.hpp"

const std::size_t size = 1024 * 1024;
extern Cache cache;
extern my_array<size>& x;
extern my_array<size>& y;
extern my_array<size>& z;
extern my_array<size>& xx;
extern my_array<size>& yy;
extern my_array<size>& zz;


void execute::f(double w) {
    
    for (unsigned int i = 0; i < size; ++i) {
        x[i] = xx[i] * w + x[i];
        y[i] = yy[i] * w + y[i];
        z[i] = zz[i] * w + z[i];
    }

}

void execute::f2(double w) {
    for (unsigned int i = 0; i < size; ++i) {
        x[i] = xx[i] * w + x[i];
    }
    for (unsigned int i = 0; i < size; ++i) {
        y[i] = yy[i] * w + y[i];
    }
    for (unsigned int i = 0; i < size; ++i) {
        z[i] = zz[i] * w + z[i];
    }
}