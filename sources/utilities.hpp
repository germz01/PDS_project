#include "CImg.h"
#include <atomic>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <string>
#include <vector>

using namespace cimg_library;

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
