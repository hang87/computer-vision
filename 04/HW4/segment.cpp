#include "CImg.h"
#include "segment.h"
#include <iostream>
#include <cmath>
#include <string>
#include <list>
#include <algorithm>
#include <queue>

using namespace cimg_library;
using namespace std;

int SEGMENT::dir[8][2] = { -1,0,1,0,0,-1,0,1,-1,-1,-1,1,1,-1,1,1 };
unsigned char SEGMENT::red[3] = { 255, 0, 0 };
unsigned char SEGMENT::blue[3] = { 0, 0, 255 };

SEGMENT::SEGMENT(string filename) {
	loadDigit();
    src_img = CImg<unsigned char>(filename.c_str());
    erosion_img = src_img;
    axis_img = src_img;
	result_img = src_img;
    width = src_img._width;
    height = src_img._height;
	save_num = 0;
    visited.resize(width, height);
}

void SEGMENT::loadDigit() {
	for (int i = 0; i < 9; i++) {
		string img_path = "./input/" + to_string(i) + ".bmp";
		digits[i] = CImg<unsigned char>(img_path.c_str());
		if (int(digits[i]._width) > digit_width)	digit_width = digits[i]._width;
		if (int(digits[i]._height) > digit_height)	digit_height = digits[i]._height;
		
	}
	for (int i = 0; i < 9; i++) {
		digits[i].resize(digit_width, digit_height);
	}
}

void SEGMENT::run() {
    Erosion(erosion_img, erosion_img, 3);
	//erosion_img.save("./output/erosion.bmp");
	markAxis(15, 50);
	segmentationFigure(300, 80);
	result_img.display();
    result_img.save("./output/ocr.bmp");
	system("pause");
}

void SEGMENT::Erosion(CImg<unsigned char> in, CImg<unsigned char> &out, int n) {
    cimg_forXY(out, i, j) {
		bool flag = false;
		for (int x = -n / 2; x <= n / 2; x++) {
			if (flag) break;
			for (int y = -n / 2; y <= n / 2; y++) {
				int xx = i + x, yy = j + y;
				if (InImage(xx, yy) && !in(xx, yy)) {
					flag = true;
					break;
				}
			}
		}
		out(i, j, 0) = out(i, j, 1) = out(i, j, 2) = flag ? 0 : 255;
	}
}

bool SEGMENT::InImage(int x, int y) {
    return x >= 0 && x < width && y >= 0 && y < height;
}

bool SEGMENT::InAxis(int x, int y) {
	return x >= axis_lx && x <= axis_rx && y >= axis_ly && y <= axis_ry;
}

void SEGMENT::segmentationFigure(int upper, int lower) {
    cimg_forXY(visited, i, j) {
		visited(i, j) = 0;
	}

    cimg_forXY(erosion_img, i, j) {
		if (!erosion_img(i, j) && !visited(i, j)) {
			cnt = 1, lx = rx = i, ly = ry = j;
			pos.clear();
			FindConnection(i, j);
			if (cnt > upper) {
				DeleteConnection();
			}
			else if (cnt > lower) {
				int side = 2;
				lx -= side, ly -= side, rx += side, ry += side;
				if (InAxis(lx, ly)) {
					ocr(lx, ly, rx, ry);
				}
				result_img.draw_line(lx, ly, rx, ly, red);
				result_img.draw_line(lx, ly, lx, ry, red);
				result_img.draw_line(rx, ly, rx, ry, red);
				result_img.draw_line(lx, ry, rx, ry, red);
			}
		}
	}
}

void SEGMENT::FindConnection(int x, int y) {
    pos.push_back(Point(x, y));
	lx = min(lx, x);
	ly = min(ly, y);
	rx = max(rx, x);
    ry = max(ry, y);
    visited(x, y) = 1;
    for (int i = 0; i < 8; i++) {
		int xx = dir[i][0] + x;
		int yy = dir[i][1] + y;
		if (InImage(xx, yy) && !visited(xx, yy) && !erosion_img(xx, yy)) {
			++cnt;
			FindConnection(xx, yy);
		}
	}
}

void SEGMENT::DeleteConnection() {
    for (Point p : pos) {
		erosion_img(p.x, p.y, 0) = erosion_img(p.x, p.y, 1) = erosion_img(p.x, p.y, 2) = 255;
	}
}

void SEGMENT::markAxis(int upper, int lower) {
    int axisY = 0;
    int max = 0;
    cimg_forY(src_img, y) {
        int count = 0;
        cimg_forX(src_img, x) {
            if (!src_img(x, y))   count++;
        }
        if (count > max) {
            max = count;
            axisY = y;
        }
    }

    int lx = 0, rx = 0;
    bool getL = false, getR = false;
    cimg_forX(src_img, x) {
        if (!getL && (!src_img(x, axisY) || !src_img(x, axisY-1) || !src_img(x, axisY+1))) {
            lx = x;
            getL = true;
        }

        if (!getR && (!src_img(width - x + 1, axisY) || !src_img(width - x + 1, axisY-1) || !src_img(width - x + 1, axisY+1))) {
            rx = width - x + 1;
            getR = true;
        }
    }
    
    axis_lx = lx, axis_ly = axisY-upper, axis_rx = rx, axis_ry = axisY+lower;
	result_img.draw_line(lx, axisY-upper, rx, axisY-upper, blue);
	result_img.draw_line(lx, axisY+lower, rx, axisY+lower, blue);
	result_img.draw_line(rx, axisY-upper, rx, axisY+lower, blue);
	result_img.draw_line(lx, axisY-upper, lx, axisY+lower, blue);
}

void SEGMENT::ocr(int lx, int ly, int rx, int ry) {
	int width = rx - lx, height = ry - ly;
	CImg<unsigned char> temp(width, height);
	cimg_forXY(temp, x, y) {
		temp(x, y) = src_img(lx + x, ly + y);
	}
	temp.resize(digit_width, digit_height);
	int ocr_result = 0;
	float min_error = 999999;
	for (int i = 0; i < 9; i++) {
		float error = 0;
		cimg_forXY(temp, x, y) {
			error = error + abs(int(temp(x, y, 0)) - int(digits[i](x, y, 0)));
		}
		error = error / float(temp._width * temp._height);
		if (error < min_error) {
			min_error = error;
			ocr_result = i;
		}
	}
	result_img.draw_text(lx, ly-15, to_string(ocr_result).c_str(), 1, red);
}
	