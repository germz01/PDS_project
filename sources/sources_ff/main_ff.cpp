#include <cassert>
#include <chrono>
#include "../CImg.h"
#include "../utilities.hpp"
#include <cstdlib>
#include <ff/farm.hpp>
#include <functional>
#include <iostream>
#include <queue>
#include <string>
#include <thread>
#include <vector>

using namespace cimg_library;
using namespace ff;

void fill_queue(std::string images_directory, std::queue<std::string>& img_queue) {
    for (auto& path : fs::directory_iterator(images_directory)) {
        std::string fname = path.path().string().substr(path.path().string().find_last_of('/') + 1);

        if (fname != ".DS_Store") {
            img_queue.push(path.path().string());
        }
    }
}

class Emitter: public ff_node {
    public:
        Emitter(int par_degree, int delay, std::queue<std::string>& img_names): par_degree(par_degree),
        delay(delay), img_names(img_names) {}

        void * svc(void *) {
            while (!(img_names.empty())) {
                image_t * t = (image_t *) calloc(1, sizeof(image_t));

                auto loading_time_start = std::chrono::high_resolution_clock::now();
                CImg<unsigned char> *img = new CImg<unsigned char>(img_names.front().c_str());
                auto loading_time_end = std::chrono::high_resolution_clock::now() - loading_time_start;
                auto loading_time = std::chrono::duration_cast<std::chrono::microseconds>(loading_time_end). \
                                    count();
                LOADING_TIME.push_back(loading_time);

                std::string fname = img_names.front().substr(img_names.front().find_last_of('/') + 1);

                t -> image = img;
                t -> name = fname;

                img_names.pop();
                std::this_thread::sleep_for(std::chrono::microseconds(delay));

                ff_send_out(t);
            }

            return NULL;
        }

    private:
        int par_degree;
        int delay;
        std::queue<std::string>& img_names;
};

class Worker: public ff_node {
    public:
        Worker(std::vector<point_t>& watermark, std::string output_dir): watermark(watermark),
        output_dir(output_dir) {}

        void * svc(void * task) {
            image_t * t = (image_t *) task;

            auto latency_start = std::chrono::high_resolution_clock::now();
            apply_watermark(*(t -> image), std::ref(watermark));
            auto latency_end = std::chrono::high_resolution_clock::now() - latency_start;
            auto latency = std::chrono::duration_cast<std::chrono::microseconds>(latency_end).count();

            {
                std::lock_guard<std::mutex> lk(LAT_MUTEX);
                LATENCIES.push_back(latency);
            }

            auto saving_time_start = std::chrono::high_resolution_clock::now();
            (t -> image) -> save((output_dir + "/" + (t -> name)).c_str());
            auto saving_time_end = std::chrono::high_resolution_clock::now() - saving_time_start;
            auto saving_time = std::chrono::duration_cast<std::chrono::microseconds>(saving_time_end).count();

            {
                std::lock_guard<std::mutex> lk(SAV_MUTEX);
                SAVING_TIME.push_back(saving_time);
            }

            PROCESSED_IMAGES += 1;

            delete t;

            return GO_ON;
        }

    private:
        std::vector<point_t>& watermark;
        std::string output_dir;
};

int main(int argc, char const *argv[]) {
    auto completion_time_start = std::chrono::high_resolution_clock::now();

    assert((argc == 6) && fs::exists(argv[1]) && fs::exists(argv[2]));

    CImg<unsigned char> watermark(argv[2]);
    int par_degree = std::atoi(argv[3]), delay = std::atoi(argv[5]);
    std::vector<point_t> black_pixels;
    std::queue<std::string> img_names;

    fill_queue((std::string)argv[1], std::ref(img_names));
    parse_watermark(std::ref(black_pixels), std::ref(watermark));
    check_output_dir((std::string)argv[4]);

    ff_farm<> farm;

    Emitter e(par_degree, delay, std::ref(img_names));
    farm.add_emitter(&e);

    std::vector<ff_node *> workers;

    for (int i = 0; i < par_degree; i++) {
        auto creation_time_start = std::chrono::high_resolution_clock::now();
        workers.push_back(new Worker(std::ref(black_pixels), (std::string)argv[4]));
        auto creation_time_end = std::chrono::high_resolution_clock::now() - creation_time_start;
        auto creation_time = std::chrono::duration_cast<std::chrono::microseconds>(creation_time_end).count();
        CREATION_TIME.push_back(creation_time);
    }

    farm.add_workers(workers);

    if (farm.run_and_wait_end() < 0) {
        return -1;
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
