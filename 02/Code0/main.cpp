#include "CImg.h"
#include "canny.h"
#include <iostream>
#include <string>

using namespace std;
using namespace cimg_library;

int main() {
	Canny image1("1.bmp");
	image1.canny_edge_detection(2.0, 0.25, 0.75, 20);
	
	// Canny image1("3.bmp");
	// image1.canny_edge_detection(2.0, 0.25, 0.75, 20);

	// Canny image2("4.bmp");
	// image2.canny_edge_detection(2.0, 0.25, 0.75, 20);

	// Canny image3("20160326110137505.bmp");
	// image3.canny_edge_detection(2.0, 0.25, 0.75, 20);

	// Canny image4("bigben.bmp");
	// image4.canny_edge_detection(2.0, 0.25, 0.75, 20);

	// Canny image5("lena.bmp");
	// image5.canny_edge_detection(2.0, 0.25, 0.75, 20);

	// Canny image6("stpietro.bmp");
	// image6.canny_edge_detection(2.0, 0.25, 0.75, 20);

	// Canny image7("lena.bmp");
	// image7.canny_ablation_experiment(2.0, 0.25, 0.75, 10, "distance1-");
	// Canny image8("lena.bmp");
	// image8.canny_ablation_experiment(2.0, 0.25, 0.75, 20, "distance2-");
	// Canny image9("lena.bmp");
	// image9.canny_ablation_experiment(2.0, 0.25, 0.75, 30, "distance3-");
	// Canny image10("lena.bmp");
	// image10.canny_ablation_experiment(2.0, 0.25, 0.75, 40, "distance4-");
	return 0;
}
