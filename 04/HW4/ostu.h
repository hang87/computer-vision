#pragma once
#ifndef OSTU_H
#define OSTU_H

#include "CImg.h"
#include <string>
using namespace cimg_library;
using namespace std;

// https://linjiafengyang.github.io/2018/05/20/Ex5%EF%BC%9A%E5%9B%BE%E5%83%8F%E5%88%86%E5%89%B2/
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