#include "CImg.h"
#include "ostu.h"
#include <iostream>
#include <cmath>
#include <string>

using namespace cimg_library;
using namespace std;

OSTU::OSTU(string filename) {
    CImg<float> image_(filename.c_str());
    image = image_;
    getThreshold();
    segment();
    image.save("result.bmp");
}

void OSTU::getThreshold() {
    double variance = 0, P1, P2, m1, m2;
    double histogram[256];

	for (int i = 0; i < 256; i++) {
		histogram[i] = 0;
	}
	int pixelsNum = image._width * image._height;

	cimg_forXY(image, i, j) {
		++histogram[int(image(i, j, 0))];
	}

	for (int i = 0; i < 256; i++) {
		P1 = 0; P2 = 0; m1 = 0; m2 = 0;

		for (int j = 0; j <= i; j++) {
			P1 += histogram[j];
			m1 += j * histogram[j];
		}
		if (P1 == 0) continue;
		m1 /= P1;
		P1 /= pixelsNum;

		for (int j = i + 1; j < 256; j++) {
			P2 += histogram[j];
			m2 += j * histogram[j];
		}
		if (P2 == 0) continue;
		m2 /= P2;
		P2 /= pixelsNum;

		double temp_variance = P1 * P2 * (m1 - m2) * (m1 - m2); 
		if (variance < temp_variance) {
			variance = temp_variance;
			threshold = i;
		}
	}
}

void OSTU::segment() {
    cimg_forXY(image, i, j) {
		if (image(i, j) > threshold) {
			image(i, j, 0, 0) = 255;
			image(i, j, 0, 1) = 255;
			image(i, j, 0, 2) = 255;
		}
		else {
			image(i, j, 0, 0) = 0;
			image(i, j, 0, 1) = 0;
			image(i, j, 0, 2) = 0;
		}
	}
}