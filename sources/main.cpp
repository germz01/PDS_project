#include <atomic>
#include <chrono>
#include "CImg.h"
#include <cstdlib>
#include <experimental/filesystem>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <typeinfo>

using namespace cimg_library;

#define IMG_NUM 44
#define W 1024
#define H 768

namespace fs = std::experimental::filesystem;

std::mutex IMAGES_MUTEX;
std::atomic_int PROCESSED_IMAGES = 0;
double OVERHEAD_TIME = 0.0;

void apply_watermark(std::queue<std::string>& images, CImg<unsigned char>& watermark, int workload, \
                     std::string output_dir) {
    std::queue<std::string> to_process;
    int counter = 0;
    bool stop = false;
    CImg<unsigned char> img;

    while (!stop) {
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

        counter = 0;

        while(!(to_process.empty())) {
            img.assign(to_process.front().c_str());

            if (!(img.width() != 1024 || img.height() != 768)) {
                cimg_forXY(watermark, x, y) {
                    int R = (int)watermark(x, y, 0, 0);

                    if (R != 255) {
                        img(x, y, 0, 0) = 0;
                        img(x, y, 0, 1) = 0;
                        img(x, y, 0, 2) = 0;
                    }
                }

                std::string fname = to_process.front().substr(to_process.front().find_last_of('/') + 1);

                try {
                    img.save_jpeg(((std::string)output_dir + (std::string)"/" + fname).c_str());
                } catch (CImgIOException e) {
                    img.save_jpeg(((std::string)output_dir + (std::string)"/" + fname).c_str());
                }

                PROCESSED_IMAGES += 1;
                to_process.pop();
            } else {
                to_process.pop();
            }
            img.clear();
        }
    }
    return;
}

int main(int argc, char const *argv[]) {
    auto completion_time_start = std::chrono::high_resolution_clock::now();
    if (argc != 5) {
        std::cout << "MISSING PARAMETERS!" << std::endl;

        return -1;
    }

    int par_degree = std::atoi(argv[3]);

    if (!(fs::exists(argv[2]))) {
        std::cout << "WATERMARK NOT FOUND!" << std::endl;

        return -1;
    }

    CImg<unsigned char> watermark(argv[2]);
    std::queue<std::string> images;

    if (!(fs::exists(argv[1]))) {
        std::cout << "IMAGES' DIRECTORY NOT FOUND!" << std::endl;

        return -1;
    }

    for (auto& path : fs::directory_iterator(argv[1])) {
        std::string fname = path.path().string().substr(path.path().string().find_last_of('/') + 1);

        if (fname != ".DS_Store") {
            images.push(path.path().string());
        }
    }

    if (!(fs::exists((std::string)argv[4]))) {
        fs::create_directory((std::string)argv[4]);
    }

    if (par_degree == 1) {
        while(!(images.empty())) {
            CImg<unsigned char> img(images.front().c_str());

            if (!(img.width() != W || img.height() != H)) {
                cimg_forXY(watermark, x, y) {
                    int R = (int)watermark(x, y, 0, 0);

                    if (R != 255) {
                        img(x, y, 0, 0) = 0;
                        img(x, y, 0, 1) = 0;
                        img(x, y, 0, 2) = 0;
                    }
                }

                std::string fname = images.front().substr(images.front().find_last_of('/') + 1);

                try {
                    img.save_jpeg(((std::string)argv[4] + (std::string)"/" + fname).c_str());
                } catch (CImgIOException e) {
                    img.save_jpeg(((std::string)argv[4] + (std::string)"/" + fname).c_str());
                }

                images.pop();
                PROCESSED_IMAGES += 1;
            } else {
                images.pop();
            }
        }
    } else if (par_degree > IMG_NUM){
        std::cout << "THE PARALLELISM DEGREE CAN'T BE GREATER THAN " << IMG_NUM << std::endl;

        return -1;
    } else {
        int workload = (int)images.size() / par_degree;
        std::thread workers[par_degree];

        for (int i = 0; i < par_degree; i++) {
            workers[i] = std::thread(apply_watermark, std::ref(images), std::ref(watermark), workload, \
                                     (std::string)argv[4]);
        }

        for (int i = 0; i < par_degree; i++) {
            workers[i].join();
        }
    }

    auto completion_time_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::ratio<1>> completion_time = completion_time_end - \
                                                                   completion_time_start;

    std::cout << "\nPARALLELISM DEGREE: " << par_degree << std::endl;
    std::cout << "COMPLETION TIME: " << completion_time.count() << " SECONDS" << std::endl;
    std::cout << "TOTAL OVERHEAD TIME: " << OVERHEAD_TIME << " SECONDS" << std::endl;
    std::cout << "PROCESSED IMAGES: " << PROCESSED_IMAGES << std::endl;

    return 0;
}
