#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <chrono>
#include <string>
#include <cstdint>
#include <istream>
#include <iosfwd>

#include <omp.h>

// TODO
// 1. num_threads
// 3. schedule

struct PNMFile {
    unsigned char magic_number[2];
    uint64_t width;
    uint64_t height;
    uint64_t max_value;
    std::string data;
};

PNMFile get_file(std::ifstream& input);
void transform_gray_map(PNMFile& file);
void transform_pix_map(PNMFile& file);
char map_pixels(char p, unsigned char old_min, unsigned char old_max);
void write_to_file(PNMFile const& file, std::ofstream& output);
void print_time(uint64_t num_threads, long long time_ms);

int main(int argc, char** argv) {

    if (argc != 4) {
        std::cerr << "Too few / much arguments" << std::endl;
        return 1;
    }

    std::ifstream input(argv[2], std::ios::binary);
    std::ofstream output(argv[3], std::ios::binary);

    if (!input || !output) {
        std::cerr << "Can't open (create) input (output) file(s)" << std::endl;
        return 1;
    }

    int num_threads = atoi(argv[1]);

    if (num_threads != 0) {
        omp_set_num_threads(num_threads);
    } else {
        num_threads = omp_get_num_threads();
    }

    PNMFile file = get_file(input);

    if (file.magic_number[0] != 'P' || (file.magic_number[1] != '5' && file.magic_number[1] != '6')) {
        std::cerr << "Not supported format" << std::endl;
        return 1;
    }

    auto start = std::chrono::steady_clock::now();
    if (file.magic_number[1] == '5') {
        transform_gray_map(file);
    } else {
        transform_pix_map(file);
    }
    auto end = std::chrono::steady_clock::now();

    write_to_file(file, output);
    input.close();

    print_time(num_threads, std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

    return 0;
}


PNMFile get_file(std::ifstream& input) {
    PNMFile file{};
    std::stringstream str;
    str << input.rdbuf();
    str >> file.magic_number[0] >> file.magic_number[1] >> file.width >> file.height >> file.max_value;
    str >> std::ws;
    file.data = std::string(std::istreambuf_iterator<char>(str), {});

    return file;
}


void transform_gray_map(PNMFile& file) {
    unsigned char min = 255;
    unsigned char max = 0;

    #pragma omp for
    for (uint64_t i = 0; i < file.height * file.width; ++i) {
        max = std::max((unsigned char) file.data[i], max);
        min = std::min((unsigned char) file.data[i], min);
    }


    if (min != 0 && max != 255) {
        for (uint64_t i = 0; i < file.height * file.width; ++i)
            file.data[i]= map_pixels(file.data[i], min, max);
    }

}

void transform_pix_map(PNMFile& file) {
    unsigned char minR = 255, minG = 255, minB = 255;
    unsigned char maxR = 0, maxG = 0, maxB = 0;

#pragma omp parallel for
    for (uint64_t i = 0; i < file.height * file.width; i += 3) {

        maxR = std::max((unsigned char) file.data[i], maxR);
        minR = std::min((unsigned char) file.data[i], minR);

        maxG = std::max((unsigned char) file.data[i + 1], maxG);
        minG = std::min((unsigned char) file.data[i + 1], minG);

        maxB = std::max((unsigned char) file.data[i + 2], maxB);
        minB = std::min((unsigned char) file.data[i + 2], minB);
        }
        unsigned char max = std::max(maxR, std::max(maxG, maxB));
        unsigned char min = std::min(minR, std::min(minG, minB));

        if (min != 0 && max != 255) {
            uint64_t img_sz = file.width * file.height * 3;
            #pragma omp parallel for
            for (uint64_t i = 0; i < img_sz; ++i)
                file.data[i] = map_pixels(file.data[i], min, max);
        }

}

char map_pixels(char p, unsigned char old_min, unsigned char old_max) {
    unsigned char val = (((unsigned char) p - old_min) * 255 / (old_max - old_min));
    val = val < 0 ? 0 : val;
    val = val > 255 ? 255 : val;
    return (char) val;
}

void write_to_file(PNMFile const& file, std::ofstream& output) {
    std::stringstream w;
    w << "P" << file.magic_number[1] << "\n" << file.width << " " << file.height << "\n" << "255" << "\n" << file.data;
    output << w.str();
    output.close();
}

void print_time(uint64_t num_threads, long long time_ms) {
    printf("Time (%llu thread(s)): %lld ms\n", num_threads, time_ms);
}
