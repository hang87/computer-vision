#include "Hough.h"
#include <cstdlib>

int main() {
    // Hough *houghTrasform1 = new Hough("data/input/Dataset1/1.bmp", "data/output/result1/1.bmp", "line", 4);
    // Hough *houghTrasform2 = new Hough("data/input/Dataset1/2.bmp", "data/output/result1/1.bmp", "line", 4);
    // Hough *houghTrasform3 = new Hough("data/input/Dataset1/3.bmp", "data/output/result1/1.bmp", "line", 4);
    // Hough *houghTrasform4 = new Hough("data/input/Dataset1/4.bmp", "data/output/result1/1.bmp", "line", 4);
    // Hough *houghTrasform5 = new Hough("data/input/Dataset1/5.bmp", "data/output/result1/1.bmp", "line", 4);
    // Hough *houghTrasform6 = new Hough("data/input/Dataset1/6.bmp", "data/output/result1/1.bmp", "line", 4);

    // Hough *houghTrasform7 = new Hough("data/input/Dataset2/1.bmp", "result/result2/1.bmp", "circle", 7, 50, 87);
	// Hough *houghTrasform8 = new Hough("data/input/Dataset2/2.bmp", "result/result2/2.bmp", "circle", 2, 25, 30);  // edit
	// Hough *houghTrasform9 = new Hough("data/input/Dataset2/3.bmp", "result/result2/3.bmp", "circle", 4, 190, 235);
	// Hough *houghTrasform10 = new Hough("data/input/Dataset2/4.bmp", "result/result2/4.bmp", "circle", 7, 165, 175);
	// Hough *houghTrasform11 = new Hough("data/input/Dataset2/5.bmp", "result/result2/5.bmp", "circle", 3, 183, 190);
	// Hough *houghTrasform12 = new Hough("data/input/Dataset2/6.bmp", "result/result2/6.bmp", "circle", 2, 470, 500); // edit
	// Hough *houghTrasform13 = new Hough("data/input/Dataset2/7.bmp", "result/result2/7.bmp", "circle", 2, 35, 40);
	// Hough *houghTrasform14 = new Hough("data/input/Dataset2/8.bmp", "result/result2/8.bmp", "circle", 2, 32, 35);
	// Hough *houghTrasform15 = new Hough("data/input/Dataset2/9.bmp", "result/result2/9.bmp", "circle", 4, 30, 40);   // edit
	// Hough *houghTrasform16 = new Hough("data/input/Dataset2/10.bmp", "result/result2/10.bmp", "circle", 3, 15, 20);

    Hough *houghTrasform1 = new Hough("data/input/Dataset3/1.bmp", "data/output/result1/1.bmp", "extra", 15);
    Hough *houghTrasform2 = new Hough("data/input/Dataset3/2.bmp", "data/output/result1/1.bmp", "extra", 15);
    Hough *houghTrasform3 = new Hough("data/input/Dataset3/3.bmp", "data/output/result1/1.bmp", "extra", 15);
    
    return 0;
}