#include <atomic>
#include <cassert>
#include <chrono>
#include "../CImg.h"
#include <cstdlib>
#include <experimental/filesystem>
#include <ff/parallel_for.hpp>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <typeinfo>

using namespace cimg_library;
using namespace ff;
namespace fs = std::experimental::filesystem;

#define IMG_NUM 44
#define W 1024
#define H 768

std::mutex IMAGES_MUTEX;
std::atomic_int PROCESSED_IMAGES = 0;
double OVERHEAD_TIME = 0.0;

void apply_watermark(std::queue<std::string>& images, CImg<unsigned char>& watermark, int workload, \
                     std::string output_dir) {
    std::queue<std::string> to_process;
    int counter = 0;
    bool stop = false;
    CImg<unsigned char> img;

    while(!stop) {
        IMAGES_MUTEX.lock();
        auto ot_start = std::chrono::high_resolution_clock::now();
        while(counter < workload) {
            if (images.empty()) {
                counter = workload;
                stop = true;
            } else {
                to_process.push(images.front());
                images.pop();
                counter += 1;
            }
        }
        auto ot_end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::ratio<1>> ot_time = ot_end - ot_start;
        OVERHEAD_TIME += ot_time.count();
        IMAGES_MUTEX.unlock();
    }
}

int main(int argc, char const *argv[]) {
    auto completion_time_start = std::chrono::high_resolution_clock::now();

    assert((argc == 5) && fs::exists(argv[2]) && fs::exists(argv[1]));

    int par_degree = std::atoi(argv[3]);

    assert(par_degree <= IMG_NUM);

    CImg<unsigned char> watermark(argv[2]);
    std::queue<std::string> images;

    for (auto& path : fs::directory_iterator(argv[1])) {
        std::string fname = path.path().string().substr(path.path().string().find_last_of('/') + 1);

        if (fname != ".DS_Store") {
            images.push(path.path().string());
        }
    }

    if (!(fs::exists((std::string)argv[4]))) {
        fs::create_directory((std::string)argv[4]);
    }

    int workload = (int)images.size() / par_degree;
    std::thread workers[par_degree];

    for (int i = 0; i < par_degree; i++) {
        workers[i] = std::thread(apply_watermark, std::ref(images), std::ref(watermark), workload, \
                                 (std::string)argv[4]);
    }

    for (int i = 0; i < par_degree; i++) {
        workers[i].join();
    }

    auto completion_time_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::ratio<1>> completion_time = completion_time_end - \
                                                                   completion_time_start;

    std::cout << "COMPLETION TIME: " << completion_time.count() << " SECONDS" << std::endl;

    return 0;
}
