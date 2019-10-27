#pragma once
#ifndef SEGMENT_H
#define SEGMENT_H

#include "CImg.h"
#include <string>
#include <vector>
#include <list>


using namespace cimg_library;
using namespace std;

struct Point {
	int x, y;
	Point(int a, int b) {
		x = a;
		y = b;
	}
};

class SEGMENT {
public:
    SEGMENT(string);
    void run();

private:
	void loadDigit();
    void Erosion(CImg<unsigned char>, CImg<unsigned char> &, int);
    bool InImage(int, int);
    void segmentationFigure(int, int);
    void FindConnection(int, int);
    void DeleteConnection();
    void markAxis(int, int);
	bool InAxis(int , int);
    void ocr(int, int, int, int);

private:
    CImg<unsigned char> src_img;
    CImg<unsigned char> erosion_img;
    CImg<unsigned char> axis_img;
	CImg<unsigned char> axis;
    CImg<unsigned char> result_img;
    CImg<bool> visited;
	CImg<unsigned char> digits[10];
    int width;
    int height;
    int cnt;
    int lx, rx, ly, ry;
	int save_num;
	int digit_width, digit_height;
    int axis_rx, axis_ry, axis_lx, axis_ly;  // up_left point, down_right point
    list<Point> pos;
    static int dir[8][2];
	static unsigned char red[3];
	static unsigned char blue[3];
};


#endif