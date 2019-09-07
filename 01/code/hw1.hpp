#include "CImg.h" 
#include <math.h>

#define _USE_MATH_DEFINES
# define M_PI           3.14159265358979323846  /* pi */
#define dis(x, y, x0, y0) (sqrt(pow((x) - (x0), 2) + pow((y) - (y0), 2)))

using namespace cimg_library;


class HW1 {
public:
	static void task1(CImg<unsigned char>& image) {
		image.display("task1");
		return;
	}
	
	static void task2(CImg<unsigned char>& image) {
		cimg_forXY(image, x, y) {
			if (image(x, y, 0) == 255 && image(x, y, 1) == 255 && image(x, y, 2) == 255) {
				image(x, y, 1) = 0;
				image(x, y, 2) = 0;
			}
			if (image(x, y, 0) == 0 && image(x, y, 1) == 0 && image(x, y, 2) == 0) {
				image(x, y, 1) = 255;
			}
		}
		
		return;
	}
	
	static void task3(CImg<unsigned char>& image) {		
		double Ax = 50;
		double Ay = 50 - 20 / cos(M_PI/6);
		double Bx = 50 - 20;
		double By = 50 + 20 * tan(M_PI/6);
		double Cx = 50 + 20;
		double Cy = 50 + 20 * tan(M_PI/6);
		
		double kab = (Ay - By) * 1.0 / (Ax - Bx);
		double lab = Ay * 1.0 - kab * Ax;
		double kbc = (By - Cy) * 1.0 / (Bx - Cx);
		double lbc = By * 1.0 - kbc * Bx;
		double kac = (Ay - Cy) * 1.0 / (Ax - Cx);
		double lac = Ay * 1.0 - kac * Ax;
		
		cimg_forXY(image, x, y) {
			// y = kx + l   ==> y - kx - l
			double dab = y - kab * x - lab;
			double dbc = y - kbc * x - lbc;
			double dac = y - kac * x - lac;
			
			if (dab >= 0 && dbc <= 0 && dac >= 0) {
				image(x, y, 2) = 255;
			}
		}
		
		return;
	}
	
	static void task3_2(CImg<unsigned char>& image) {
		double Ax = 50;
		double Ay = 50 - 20 / cos(M_PI/6);
		double Bx = 50 - 20;
		double By = 50 + 20 * tan(M_PI/6);
		double Cx = 50 + 20;
		double Cy = 50 + 20 * tan(M_PI/6);
		
		unsigned char color[]    = { 0, 0, 255 };
		
		image.draw_triangle(
	      Ax, Ay,       // 1st vertex x,y
	      Bx, By,      // 2nd vertex x,y
	      Cx, Cy,     // 3rd vertex x,y 
	      color
		);

	}
	
	static void task4(CImg<unsigned char>& image) {
		cimg_forXY(image, x, y) {
			if (pow(x-50, 2) + pow(y-50, 2) <= pow(15, 2)) {
				image(x, y, 0) = 255;
				image(x, y, 1) = 255;
			}
		}
			
		return;
	}
	
	static void task4_2(CImg<unsigned char>& image) {
		unsigned char yellow[] = {255, 255, 0};
		image.draw_circle(50, 50, 15, yellow);
			
		return;
	}
	
	
	static void task5(CImg<unsigned char>& image) {
		cimg_forXY(image, x, y) {
			if (x == y && dis(x, y, 0, 0) <= 100) {
				image(x, y, 0) = 0;
				image(x, y, 1) = 255;
				image(x, y, 2) = 0;
			}
		}
		
		return;
	}
	
	static void task5_2(CImg<unsigned char>& image) {
		unsigned char blue[] = {0, 255, 0};
		image.draw_line(0, 0, 100 * cos(45*M_PI/180), 100 * sin(45*M_PI/180), blue);
		
		return;
	}
	
	static void task6(CImg<unsigned char>& image) {
		image.save("2.bmp");
	}
}; 
