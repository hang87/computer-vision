#include "CImg.h"
#include "canny.h"
#include <iostream>
#include <string>

using namespace std;
using namespace cimg_library;

int main() {
	Canny image1("3.bmp");
	image1.canny_edge_detection(2.0, 0.25, 0.75, 20);

	Canny image2("4.bmp");
	image2.canny_edge_detection(2.0, 0.25, 0.75, 20);

	Canny image3("20160326110137505.bmp");
	image3.canny_edge_detection(2.0, 0.25, 0.75, 20);

	Canny image4("bigben.bmp");
	image4.canny_edge_detection(2.0, 0.25, 0.75, 20);

	Canny image5("lena.bmp");
	image5.canny_edge_detection(2.0, 0.25, 0.75, 20);

	Canny image6("stpietro.bmp");
	image6.canny_edge_detection(2.0, 0.25, 0.75, 20);
	return 0;
}
