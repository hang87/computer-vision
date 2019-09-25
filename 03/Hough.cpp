#include "CImg.h"
#include "Hough.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;
using namespace cimg_library;

Hough::Hough(double sigma, double gra_thred, double vote_thred, double peak_dis, string path) {
    CImg<int> image(path.c_str());
    this->img = image;
    this->result_img = image;
    this->sigma = sigma;
    this->gra_thred = gra_thred;
    this->vote_thred = vote_thred;
    this->peak_dis = peak_dis;
    this->x_min = 0;
	this->x_max = img._width - 1; 
	this->y_min = 0;
	this->y_max = img._height - 1; 
}

void Hough::RGB2Gray() {
    int r = 0, g =0, b = 0;
	cimg_forXY(img, x, y) {
		r = img(x, y, 0);
		g = img(x, y, 1);
		b = img(x, y, 2);
		img(x, y, 0) = img(x, y, 1) = img(x, y, 2) = (r*0.2 + g * 0.7 + b * 0.1);
	}
    img.resize(img.width(), img.height(), 1, 1, 5);
}

void Hough::guassionFilter() {
    blur_img = img.get_blur(sigma);
}

void Hough::Cartesian2Hough() {
    CImg<float> sobelx(3, 3, 1, 1, 0);
	CImg<float> sobely(3, 3, 1, 1, 0);
    int sobel_xs[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    int sobel_ys[9] = {1, 2, 1, 0, 0, 0, -1, -2, -1};
    cimg_forXY(sobelx, i, j) {
        sobelx(i, j, 0) = sobel_xs[i+j];
    }
    cimg_forXY(sobely, i, j) {
        sobely(i, j, 0) = sobel_ys[i+j];
    }

    CImg<float> gradient_x = blur_img;
    gradient_x = gradient_x.get_convolve(sobelx);
    CImg<float> gradient_y = blur_img;
    gradient_y = gradient_y.get_convolve(sobely);

    int maxp = (int)sqrt(img._width * img._width + img._height * img._height);
	CImg<float> hough_space(360, maxp, 1, 1, 0);

    cimg_forXY(img, i, j) {
		double grad = sqrt(gradient_x(i, j)*gradient_x(i, j) + gradient_y(i, j)*gradient_y(i, j));
		if (grad > gra_thred) {
			img(i, j) = grad;
			cimg_forX(hough_space, alpha) {
				double theta = ((double)alpha*cimg::PI) / 180;
				int p = (int)(i*cos(theta) + j*sin(theta));
				if (p >= 0 && p < maxp) {
					hough_space(alpha, p)++;
				}
			}
		}
	}

    this->hough_space = hough_space;
}

void Hough::fingPeeks() {
    peaks.clear();
	cimg_forXY(hough_space, theta, p) {
		if (hough_space(theta, p) > vote_thred) {
			bool flag = true;
			double alpha = (double)theta*cimg::PI / 180;

			const int y0 = ((double)p / (sin(alpha))) - double(x_min)*(1 / tan(alpha));
			const int y1 = ((double)p / (sin(alpha))) - double(x_max)*(1 / tan(alpha));

			const int x0 = ((double)p / (cos(alpha))) - double(y_min)*(tan(alpha));
			const int x1 = ((double)p / (cos(alpha))) - double(y_max)*(tan(alpha));

			if (x0 >= x_min && x0 <= x_max || x1 >= x_min && x1 <= x_max ||
				y0 >= y_min && y0 <= y_max || y1 >= y_min && y1 <= y_max) {
				for (int i = 0; i < peaks.size(); i++) {
					if (sqrt((peaks[i].x - theta)*(peaks[i].x - theta) 
						+ (peaks[i].y - p)*(peaks[i].y - p)) < peak_dis) {
						flag = false;
						if (peaks[i].cnt < hough_space(theta, p)) {
							Point temp(theta, p, hough_space(theta, p));
							peaks[i] = temp;
						}
					}
				}
				if (flag) {
					Point temp(theta, p, hough_space(theta, p));
					peaks.push_back(temp);
				}
			}
		}
	}
}

void Hough::drawLine() {
    lines.clear();
	for (int i = 0; i < peaks.size(); i++) {
		double theta = double(peaks[i].x)*cimg::PI / 180;
		double k = -cos(theta) / sin(theta);
		double b = double(peaks[i].y) / sin(theta);
		Line templine(k, b);
		lines.push_back(templine);
		// cout << "Line " << i << ": y = " << k << "x + " << b << endl;
	}

	const double lines_color[] = { 255, 0, 0 };
	for (int i = 0; i < lines.size(); i++) {
		const int x0 = (double)(y_min - lines[i].b) / lines[i].k;
		const int x1 = (double)(y_max - lines[i].b) / lines[i].k;
		const int y0 = x_min*lines[i].k + lines[i].b;
		const int y1 = x_max*lines[i].k + lines[i].b;

		if (abs(lines[i].k) > 1) {
			result_img.draw_line(x0, y_min, x1, y_max, lines_color);
		}
		else {
			result_img.draw_line(x_min, y0, x_max, y1, lines_color);
		}
	}
}

void Hough::drawIntersections() {
	intersections.clear();
	int k = 0;
	for (int i = 0; i < lines.size(); i++) {
		for (int j = i + 1; j < lines.size(); j++) {
			double k0 = lines[i].k;
			double k1 = lines[j].k;
			double b0 = lines[i].b;
			double b1 = lines[j].b;

			double x = (b1 - b0) / (k0 - k1);
			double y = (k0*b1 - k1*b0) / (k0 - k1);

			if (x >= 0 && x < img._width && y >= 0 && y < img._height) {
				Point tempPoint(x, y, 0);
				intersections.push_back(tempPoint);
				cout << "Intersection " << k++ << ": x = " << x << ", y = " << y << endl;
			}
		}
	}

	const double intersections_color[] = { 255, 0, 0 };
	for (int i = 0; i < intersections.size(); i++) {
		result_img.draw_circle(intersections[i].x, intersections[i].y, 50, intersections_color);
	}

    result_img.display();
}