#pragma once
#ifndef OSTU_H
#define OSTU_H

#include "CImg.h"
#include <string>
using namespace cimg_library;
using namespace std;

class OSTU {
public:
    OSTU(string);
    void getThreshold();
    void segment();
private:
    CImg<float> image;
    int threshold;
};


#endif