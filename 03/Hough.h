#include "CImg.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;
using namespace cimg_library;

struct Point {
	int x, y, cnt;
	Point(int _x, int _y, int _cnt): x(_x), y(_y), cnt(_cnt) {}
};

struct Line {
	double k, b;
	Line(double _k, double _b): k(_k), b(_b) {}
};

class Hough {
private:
    CImg<int> img;
    CImg<int> result_img;
    CImg<int> blur_img;
    CImg<float> hough_space;
    double sigma;
    double gra_thred;
    double vote_thred;
    double peak_dis;
    int x_min, x_max, y_min, y_max;
    vector<Point> peaks;
    vector<Line> lines;
    vector<Point> intersections;
public:
    Hough(double sigma, double gra_thred, double vote_thred, double peak_dis, string path);
    void RGB2Gray();
    void guassionFilter();
    void Cartesian2Hough();
    void fingPeeks();
    void drawLine();
    void drawIntersections();
};