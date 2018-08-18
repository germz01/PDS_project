#include "CImg.h"
#include <atomic>
#include <condition_variable>
#include <deque>
#include <experimental/filesystem>
#include <mutex>
#include <numeric>
#include <string>
#include <vector>

using namespace cimg_library;
namespace fs = std::experimental::filesystem;

struct point_t {
    int x;
    int y;
};

struct image_t {
    CImg<unsigned char> *image;
    std::string name;
};

std::atomic_int PROCESSED_IMAGES = 0;

std::deque<image_t*> IMAGES;
std::mutex IMAGES_MUTEX, LAT_MUTEX, SAV_MUTEX;
std::condition_variable cv;

std::vector<double> LATENCIES, LOADING_TIME, SAVING_TIME, CREATION_TIME;

void parse_watermark(std::vector<point_t>& vect, CImg<unsigned char>& watermark) {
    int current_index = 0;

    for (int x = 0; x < watermark.width(); x++) {
        for (int y = 0; y < watermark.height(); y++) {
            int r = (int)watermark(x, y, 0, 0);

            if (r == 0) {
                vect.push_back(point_t());
                vect[current_index].x = x;
                vect[current_index].y = y;
                current_index += 1;
            }
        }
    }
}

void check_output_dir(std::string path) {
    if (!(fs::exists(path))) {
        fs::create_directory(path);
    }
}

double stats(std::string statistic, std::vector<double>& vect) {
    double stat;

    if (statistic == "mean") {
        stat = std::accumulate(vect.begin(), vect.end(), 0.0)/vect.size();
    } else {
        stat = std::accumulate(vect.begin(), vect.end(), 0.0);
    }

    return stat;
}

void apply_watermark(CImg<unsigned char>& image, std::vector<point_t>& black_pixels) {
    for (point_t pixel : black_pixels) {
            image(pixel.x, pixel.y, 0, 0) = 0;
            image(pixel.x, pixel.y, 0, 1) = 0;
            image(pixel.x, pixel.y, 0, 2) = 0;
        }
}
