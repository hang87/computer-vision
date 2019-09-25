#include "CImg.h"
#include "Hough.h"
#include <iostream>
#include <string>

using namespace std;
using namespace cimg_library;

// g++ main.cpp Hough.cpp -o main -O2 -lgdi32
int main() {
    Hough image(10.5f, 30, 1000, 60, "./data/input/3.bmp");
    image.RGB2Gray();
    image.guassionFilter();
    image.Cartesian2Hough();
    image.fingPeeks();
    image.drawLine();
    image.drawIntersections();
    
    return 0;
}