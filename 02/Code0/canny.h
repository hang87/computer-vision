#ifndef CANNY_H_
#define CANNY_H_

#include <iostream>
#include <string>
#include "CImg.h"

using namespace std;
using namespace cimg_library;

class Canny {
private:
	CImg<int> img;
	string name;	// image's name
	int rows;			// image height
	int cols;			// image width
	int *smoothedim;	// image after gaussian smothing
	int *delta_x;		// derivatives in x axis
	int *delta_y;		// derivatives in y axis
	float *dir_radians;	// gradient direction
	int *magnitude;		// gradient magnitude
	int *nms;			// image after non-maximize suppression
	int *edge;			// iamge after hysterresis 
	int *c_line;		// image after connect line
	int *d_line;		// image after delete line
public:
	Canny();
	Canny(string name);
	~Canny();
	void RGB2Gray();
	void gaussian_smooth(float sigma);
	void derrivative_x_y();
	void magnitude_x_y();
	void radian_direction(int xdirtag, int ydirtag);
	void non_max_supp();
	void apply_hysteresis(float tlow, float thigh);
	void connect_line(int distance);
	void delete_line(int distance);

	void canny_edge_detection(float sigma, float tlow, float thig, int distance);
	void canny_ablation_experiment(float sigma, float tlow, float thig, int distance, string savename);
	void save_result();

	CImg<int> matrix2image(int *matrix);
	void image2matrix(CImg<int> img, int *matrix);
	void show_temp_result(int *matirx);
};

#endif
