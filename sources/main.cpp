#include <cassert>
#include <chrono>
#include <cstddef>
#include "CImg.h"
#include "utilities.hpp"
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using namespace cimg_library;

void fill_queue(std::string images_directory, int delay) {
    CImg<unsigned char> *img;

    for (auto& path : fs::directory_iterator(images_directory)) {
            std::string fname = path.path().string().substr(path.path().string().find_last_of('/') + 1);

            if (fname != ".DS_Store") {
                auto loading_time_start = std::chrono::high_resolution_clock::now();
                img = new CImg<unsigned char>(path.path().string().c_str());
                auto loading_time_end = std::chrono::high_resolution_clock::now() - loading_time_start;
                auto loading_time = std::chrono::duration_cast<std::chrono::microseconds>(loading_time_end). \
                                    count();
                LOADING_TIME.push_back(loading_time);

                image_t *to_push = new image_t();
                to_push -> image = img;
                to_push -> name = fname;

                {
                    std::lock_guard<std::mutex> lk(IMAGES_MUTEX);
                    IMAGES.push_front(to_push);
                }
                cv.notify_all();

                std::this_thread::sleep_for(std::chrono::microseconds(delay));
            }
        }

    {
        std::lock_guard<std::mutex> lk(IMAGES_MUTEX);
        IMAGES.push_front(nullptr);
    }
    cv.notify_all();
}

void process_image(std::vector<point_t>& black_pixels, std::string output_directory) {
    while(true) {
        std::unique_lock<std::mutex> lk(IMAGES_MUTEX);
        cv.wait(lk, []{return !IMAGES.empty();});

        if (IMAGES.back() == nullptr) {
            lk.unlock();
            break;
        } else {
            image_t *img = IMAGES.back();
            IMAGES.pop_back();
            lk.unlock();

            auto latency_start = std::chrono::high_resolution_clock::now();
            apply_watermark(*(img -> image), std::ref(black_pixels));
            auto latency_end = std::chrono::high_resolution_clock::now() - latency_start;
            auto latency = std::chrono::duration_cast<std::chrono::microseconds>(latency_end).count();

            {
                std::lock_guard<std::mutex> lk(LAT_MUTEX);
                LATENCIES.push_back(latency);
            }

            auto saving_time_start = std::chrono::high_resolution_clock::now();
            (img -> image) -> save((output_directory + "/" + (img -> name)).c_str());
            auto saving_time_end = std::chrono::high_resolution_clock::now() - saving_time_start;
            auto saving_time = std::chrono::duration_cast<std::chrono::microseconds>(saving_time_end).count();

            {
                std::lock_guard<std::mutex> lk(SAV_MUTEX);
                SAVING_TIME.push_back(saving_time);
            }

            delete img;
        }
        PROCESSED_IMAGES += 1;
    }
}

int main(int argc, char const *argv[]) {
    auto completion_time_start = std::chrono::high_resolution_clock::now();

    assert((argc == 6) && fs::exists(argv[1]) && fs::exists(argv[2]));

    CImg<unsigned char> watermark(argv[2]);
    int par_degree = std::atoi(argv[3]), delay = std::atoi(argv[5]);
    std::vector<point_t> black_pixels;

    parse_watermark(std::ref(black_pixels), std::ref(watermark));
    check_output_dir((std::string)argv[4]);

    if (par_degree == 0) {
        CImg<unsigned char> *img;

        for (auto& path : fs::directory_iterator(argv[1])) {
            std::string fname = path.path().string().substr(path.path().string().find_last_of('/') + 1);

            if (fname != ".DS_Store") {
		        auto loading_time_start = std::chrono::high_resolution_clock::now();
                img = new CImg<unsigned char>(path.path().string().c_str());
                auto loading_time_end = std::chrono::high_resolution_clock::now() - loading_time_start;
                auto loading_time = std::chrono::duration_cast<std::chrono::microseconds>(loading_time_end).\
                                    count();
                LOADING_TIME.push_back(loading_time);

		std::cout << "Loaded " << path.path().string().c_str() << std::endl;

                auto latency_start = std::chrono::high_resolution_clock::now();
                apply_watermark(*(img), std::ref(black_pixels));
                auto latency_end = std::chrono::high_resolution_clock::now() - latency_start;
                auto latency = std::chrono::duration_cast<std::chrono::microseconds>(latency_end).count();
                LATENCIES.push_back(latency);

		std::cout << "Applied watermark on " << path.path().string().c_str() << std::endl;
                
		auto saving_time_start = std::chrono::high_resolution_clock::now();
                img -> save(((std::string)argv[4] + (std::string)"/" + fname).c_str());
                auto saving_time_end = std::chrono::high_resolution_clock::now() - saving_time_start;
                auto saving_time = std::chrono::duration_cast<std::chrono::microseconds>(saving_time_end).\
                                   count();
                SAVING_TIME.push_back(saving_time);

		std::cout << "Saved " << path.path().string().c_str() << std::endl;

                PROCESSED_IMAGES += 1;
                std::this_thread::sleep_for(std::chrono::microseconds(delay));

		std::cout << "Updated counter" << std::endl;

		delete img;

		std::cout << "Deleted pointer" << std::endl;

		std::cout << "Ok " << path.path().string().c_str() << std::endl;
            }
        }
    } else {
        std::thread workers[par_degree];

        for (int i = 0; i < par_degree; i++) {
            auto creation_time_start = std::chrono::high_resolution_clock::now();
            workers[i] = std::thread(process_image, std::ref(black_pixels) , (std::string)argv[4]);
            auto creation_time_end = std::chrono::high_resolution_clock::now() - creation_time_start;
            auto creation_time = std::chrono::duration_cast<std::chrono::microseconds>(creation_time_end).\
                                 count();
            CREATION_TIME.push_back(creation_time);
        }

        fill_queue((std::string)argv[1], delay);

        for (int i = 0; i < par_degree; i++) {
            workers[i].join();
        }

    }

    auto completion_time_end = std::chrono::high_resolution_clock::now() - completion_time_start;
    auto completion_time = std::chrono::duration_cast<std::chrono::microseconds>(completion_time_end).count();

    std::cout << "\nPARALLELISM DEGREE: " << par_degree << std::endl;
    std::cout << "COMPLETION TIME: " << completion_time << " \u03BCs" << std::endl;
    std::cout << "MEAN LATENCY: " << stats("mean", std::ref(LATENCIES)) << " \u03BCs" << std::endl;
    std::cout << "MEAN LOADING TIME: " << stats("mean", std::ref(LOADING_TIME)) << " \u03BCs" << std::endl;
    std::cout << "MEAN SAVING TIME: " << stats("mean", std::ref(SAVING_TIME)) << " \u03BCs" << std::endl;
    std::cout << "MEAN CREATION TIME: " << stats("mean", std::ref(CREATION_TIME)) << " \u03BCs" << std::endl;
    std::cout << "PROCESSED IMAGES: " << PROCESSED_IMAGES << std::endl;

    return 0;
}
