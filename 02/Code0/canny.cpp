#include "CImg.h"
#include "canny.h"
#include <iostream>
#include <string>
#include <cstring>
#include <cmath>
#include <algorithm>

#define M_PI 3.14159265358979323846
#define BOOSTBLURFACTOR 90.0
#define NOEDGE 255
#define POSSIBLE_EDGE 128
#define EDGE 0

void make_gaussian_kernel(float sigma, float **kernel, int *windowsize);
double angle_radians(double x, double y);
void follow_edges(int *edgemapptr, int *edgemagptr, int lowval, int cols);
CImg<int> Draw_line(CImg<int> tmp, int x ,int y, int x1, int y1);

using namespace std;
using namespace cimg_library;

Canny::Canny() {
	
}

Canny::Canny(string path) {
	this->name = path;
	path = "./test_Data/" + path;
	CImg<int> image(path.c_str());
	this->img = image;

	this->rows = img.width();
	this->cols = img.height();

	delta_x = new int[rows * cols]; memset(delta_x, 0x0, rows * cols *sizeof(int));
	delta_y = new int[rows * cols]; memset(delta_y, 0x0, rows * cols * sizeof(int));
	dir_radians = new float[rows * cols]; memset(dir_radians, 0x0, rows * cols * sizeof(float));
	magnitude = new int[rows * cols]; memset(magnitude, 0x0, rows * cols * sizeof(int));
	smoothedim = new int[rows*cols];  memset(smoothedim, 0x0, rows*cols * sizeof(int));
	nms = new int[rows*cols];  memset(nms, 0x0, rows*cols * sizeof(int));
	edge = new int[rows*cols];  memset(edge, 0x0, rows*cols * sizeof(int));
	c_line = new int[rows*cols];  memset(c_line, 0x0, rows*cols * sizeof(int));
	d_line = new int[rows*cols];  memset(d_line, 0x0, rows*cols * sizeof(int));
}

Canny::~Canny() {
	delete[] delta_x;
	delete[] delta_y;
	delete[] dir_radians;
	delete[] magnitude;
	delete[] nms;
	delete[] edge;
	delete[] c_line;
	delete[] d_line;
}

void Canny::RGB2Gray() {
	int r = 0, g =0, b = 0;
	cimg_forXY(img, x, y) {
		r = img(x, y, 0);
		g = img(x, y, 1);
		b = img(x, y, 2);
		img(x, y, 0) = img(x, y, 1) = img(x, y, 2) = (r*0.2 + g * 0.7 + b * 0.1);
	}
	img.resize(rows, cols, 1, 1, 5);
}

void Canny::gaussian_smooth(float sigma) {
	float *tempim = new float[rows*cols];        /* Buffer for separable filter gaussian smoothing. */
	int r, c, rr, cc,     /* Counter variables. */
      windowsize,        /* Dimension of the gaussian kernel. */
      center;            /* Half of the windowsize. */
    float *kernel,        /* A one dimensional gaussian kernel. */
         dot,            /* Dot product summing variable. */
         sum;            /* Sum of the kernel weights variable. */
		
	make_gaussian_kernel(sigma, &kernel, &windowsize);
	center = windowsize / 2;

	for (r = 0; r < rows; r++) {
		for (c = 0; c < cols; c++) {
			dot = 0.0;
			sum = 0.0;
			for (cc = (-center); cc <= center; cc++) {
				if (((c + cc) >= 0) && ((c + cc) < cols)) {
					dot += (float)img(r,c+cc) * kernel[center + cc];
					sum += kernel[center + cc];
				}
			}
			tempim[r*cols + c] = dot / sum;
		}
	}

	for (c = 0; c < cols; c++) {
		for (r = 0; r < rows; r++) {
			sum = 0.0;
			dot = 0.0;
			for (rr = (-center); rr <= center; rr++) {
				if (((r + rr) >= 0) && ((r + rr) < rows)) {
					dot += tempim[(r + rr)*cols + c] * kernel[center + rr];
					sum += kernel[center + rr];
				}
			}
			smoothedim[r*cols + c] = (short int)(dot*BOOSTBLURFACTOR / sum + 0.5);
		}
	}
}

void make_gaussian_kernel(float sigma, float **kernel, int *windowsize) {
	int i = 0, center = 0;
	float x, fx, sum = 0.0;

	*windowsize = 1 + 2 * ceil(2.5 * sigma);		// caculate windowsize using variance
	center = (*windowsize) / 2;
	*kernel = new float[*windowsize];

	for(i=0;i<(*windowsize);i++) {
      x = (float)(i - center);
      fx = pow(2.71828, -0.5*x*x/(sigma*sigma)) / (sigma * sqrt(6.2831853));	// gaussian distribution
      (*kernel)[i] = fx;
      sum += fx;
   }

   for(i=0;i<(*windowsize);i++) {
		(*kernel)[i] /= sum;		// normalize
   }
}

void Canny::derrivative_x_y() {
	int r = 0, c = 0, pos = 0;
	// caculate derivatives of x axis
	for (r = 0; r < rows; r++) {
		pos = r * cols;
		delta_x[pos] = smoothedim[pos + 1] - smoothedim[pos];
		pos++;
		for (c = 1; c < (cols - 1); c++, pos++) {
			delta_x[pos] = smoothedim[pos + 1] - smoothedim[pos - 1];
		}
		delta_x[pos] = smoothedim[pos] - smoothedim[pos - 1];
	}
	// caculate derivatives of y axis
	for (c = 0; c < cols; c++) {
		pos = c;
		delta_y[pos] = smoothedim[pos + cols] - smoothedim[pos];
		pos += cols;
		for (r = 1; r < (rows - 1); r++, pos += cols) {
			delta_y[pos] = smoothedim[pos + cols] - smoothedim[pos - cols];
		}
		delta_y[pos] = smoothedim[pos] - smoothedim[pos - cols];
	}
}

void Canny::magnitude_x_y() {
	int r = 0, c = 0, pos = 0, sq1 = 0, sq2 = 0;
	for (r = 0, pos = 0; r < rows; r++) {
		for (c = 0; c < cols; c++, pos++) {
			sq1 = (int)delta_x[pos] * (int)delta_x[pos];
			sq2 = (int)delta_y[pos] * (int)delta_y[pos];
			magnitude[pos] = (short)(0.5 + sqrt((float)sq1 + (float)sq2));    // rounding
		}
	}
}

void Canny::radian_direction(int xdirtag, int ydirtag) {
	double dx = 0.0, dy = 0.0;
	int r = 0, c = 0, pos = 0;
	for (r = 0, pos = 0; r < rows; r++) {
		for (c = 0; c < cols; c++, pos++) {
			dx = (double)delta_x[pos];
			dy = (double)delta_y[pos];

			if (xdirtag == 1) dx = -dx;
			if (ydirtag == -1) dy = -dy;

			dir_radians[pos] = (float)angle_radians(dx, dy);
		}
	}
}

double angle_radians(double x, double y) {
	double xu = 0.0, yu = 0.0, ang = 0.0;
	xu = fabs(x);
	yu = fabs(y);
	if ((xu == 0) && (yu == 0)) return(0);
	ang = atan(yu / xu);
	if (x >= 0) {
		if (y >= 0) return (ang);
		else return(2 * M_PI - ang);
	}
	else {
		if (y >= 0) return (M_PI - ang);
		else return(M_PI + ang);
	}
}

void Canny::non_max_supp() {
	int rowcount = 0, colcount = 0, count = 0;
	int *magrowptr, *magptr;
	int *gxrowptr, *gxptr;
	int *gyrowptr, *gyptr, z1 = 0, z2 = 0;
	int m00, gx = 0, gy = 0;
	float mag1 = 0.0, mag2 = 0.0, xperp = 0.0, yperp = 0.0;
	int *resultrowptr, *resultptr;

	/****************************************************************************
   	* Zero the edges of the result image.
   	****************************************************************************/
	for (count = 0, resultrowptr = nms, resultptr = nms + cols * (rows - 1);
		count < cols; resultptr++, resultrowptr++, count++) {
		*resultrowptr = *resultptr = 0;
	}

	for (count = 0, resultptr = nms, resultrowptr = nms + cols - 1;
		count < rows; count++, resultptr += cols, resultrowptr += cols) {
		*resultptr = *resultrowptr = 0;
	}

	/****************************************************************************
	* Suppress non-maximum points.
	****************************************************************************/
	for (rowcount = 1, magrowptr = magnitude + cols + 1, gxrowptr = delta_x + cols + 1,
		gyrowptr = delta_y + cols + 1, resultrowptr = nms + cols + 1;
		rowcount < rows - 2;
		rowcount++, magrowptr += cols, gyrowptr += cols, gxrowptr += cols,
		resultrowptr += cols) {
		for (colcount = 1, magptr = magrowptr, gxptr = gxrowptr, gyptr = gyrowptr,
			resultptr = resultrowptr; colcount < cols - 2;
			colcount++, magptr++, gxptr++, gyptr++, resultptr++) {
			m00 = *magptr;
			if (m00 == 0) {
				*resultptr = NOEDGE;
			}
			else {
				xperp = -(gx = *gxptr) / ((float)m00);
				yperp = (gy = *gyptr) / ((float)m00);
			}

			if (gx >= 0) {
				if (gy >= 0) {
					if (gx >= gy)
					{
						/* 111 */
						/* Left point */
						z1 = *(magptr - 1);
						z2 = *(magptr - cols - 1);

						mag1 = (m00 - z1)*xperp + (z2 - z1)*yperp;

						/* Right point */
						z1 = *(magptr + 1);
						z2 = *(magptr + cols + 1);

						mag2 = (m00 - z1)*xperp + (z2 - z1)*yperp;
					}
					else
					{
						/* 110 */
						/* Left point */
						z1 = *(magptr - cols);
						z2 = *(magptr - cols - 1);

						mag1 = (z1 - z2)*xperp + (z1 - m00)*yperp;

						/* Right point */
						z1 = *(magptr + cols);
						z2 = *(magptr + cols + 1);

						mag2 = (z1 - z2)*xperp + (z1 - m00)*yperp;
					}
				}
				else
				{
					if (gx >= -gy)
					{
						/* 101 */
						/* Left point */
						z1 = *(magptr - 1);
						z2 = *(magptr + cols - 1);

						mag1 = (m00 - z1)*xperp + (z1 - z2)*yperp;

						/* Right point */
						z1 = *(magptr + 1);
						z2 = *(magptr - cols + 1);

						mag2 = (m00 - z1)*xperp + (z1 - z2)*yperp;
					}
					else
					{
						/* 100 */
						/* Left point */
						z1 = *(magptr + cols);
						z2 = *(magptr + cols - 1);

						mag1 = (z1 - z2)*xperp + (m00 - z1)*yperp;

						/* Right point */
						z1 = *(magptr - cols);
						z2 = *(magptr - cols + 1);

						mag2 = (z1 - z2)*xperp + (m00 - z1)*yperp;
					}
				}
			}
			else
			{
				if ((gy = *gyptr) >= 0)
				{
					if (-gx >= gy)
					{
						/* 011 */
						/* Left point */
						z1 = *(magptr + 1);
						z2 = *(magptr - cols + 1);

						mag1 = (z1 - m00)*xperp + (z2 - z1)*yperp;

						/* Right point */
						z1 = *(magptr - 1);
						z2 = *(magptr + cols - 1);

						mag2 = (z1 - m00)*xperp + (z2 - z1)*yperp;
					}
					else
					{
						/* 010 */
						/* Left point */
						z1 = *(magptr - cols);
						z2 = *(magptr - cols + 1);

						mag1 = (z2 - z1)*xperp + (z1 - m00)*yperp;

						/* Right point */
						z1 = *(magptr + cols);
						z2 = *(magptr + cols - 1);

						mag2 = (z2 - z1)*xperp + (z1 - m00)*yperp;
					}
				}
				else
				{
					if (-gx > -gy)
					{
						/* 001 */
						/* Left point */
						z1 = *(magptr + 1);
						z2 = *(magptr + cols + 1);

						mag1 = (z1 - m00)*xperp + (z1 - z2)*yperp;

						/* Right point */
						z1 = *(magptr - 1);
						z2 = *(magptr - cols - 1);

						mag2 = (z1 - m00)*xperp + (z1 - z2)*yperp;
					}
					else
					{
						/* 000 */
						/* Left point */
						z1 = *(magptr + cols);
						z2 = *(magptr + cols + 1);

						mag1 = (z2 - z1)*xperp + (m00 - z1)*yperp;

						/* Right point */
						z1 = *(magptr - cols);
						z2 = *(magptr - cols - 1);

						mag2 = (z2 - z1)*xperp + (m00 - z1)*yperp;
					}
				}
			}

			/* Now determine if the current point is a maximum point */

			if ((mag1 > 0.0) || (mag2 > 0.0))
			{
				*resultptr = NOEDGE;
			}
			else
			{
				if (mag2 == 0.0)
					*resultptr = NOEDGE;
				else
					*resultptr = POSSIBLE_EDGE;
			}
		}
	}
}

void Canny::apply_hysteresis(float tlow, float thigh) {
	int r = 0, c = 0, pos = 0, numedges = 0, lowcount = 0, highcount = 0, lowthreshold = 0, highthreshold = 0,
		i = 0, *hist, rr = 0, cc = 0;
	hist = new int[32768];
	int maximum_mag = 0, sumpix = 0;
	for (r = 0, pos = 0; r < rows; r++) {
		for (c = 0; c < cols; c++, pos++) {
			if (nms[pos] == POSSIBLE_EDGE) edge[pos] = POSSIBLE_EDGE;
			else edge[pos] = NOEDGE;
		}
	}

	for (r = 0, pos = 0; r < rows; r++, pos += cols) {
		edge[pos] = NOEDGE;
		edge[pos + cols - 1] = NOEDGE;
	}
	pos = (rows - 1) * cols;
	for (c = 0; c < cols; c++, pos++) {
		edge[c] = NOEDGE;
		edge[pos] = NOEDGE;
	}
	for (r = 0; r < 32768; r++) hist[r] = 0;
	for (r = 0, pos = 0; r < rows; r++) {
		for (c = 0; c < cols; c++, pos++) {
			if (edge[pos] == POSSIBLE_EDGE) hist[magnitude[pos]]++;
		}
	}
	for (r = 1, numedges = 0; r < 32768; r++) {
		if (hist[r] != 0) maximum_mag = r;
		numedges += hist[r];
	}

	highcount = (int)(numedges * thigh + 0.5);

	r = 1;
	numedges = hist[1];
	while ((r < (maximum_mag - 1)) && (numedges < highcount)) {
		r++;
		numedges += hist[r];
	}
	highthreshold = r;
	lowthreshold = (int)(highthreshold * tlow + 0.5);

	for (r = 0, pos = 0; r < rows; r++) {
		for (c = 0; c < cols; c++, pos++) {
			if ((edge[pos] == POSSIBLE_EDGE) && (magnitude[pos] >= highthreshold)) {
				edge[pos] = EDGE;
				follow_edges((edge + pos), (magnitude + pos), lowthreshold, cols);
			}
		}
	}

	for (r = 0, pos = 0; r < rows; r++) {
		for (c = 0; c < cols; c++, pos++) if (edge[pos] != EDGE) edge[pos] = NOEDGE;
	}
	delete[] hist;
}

void follow_edges(int *edgemapptr, int *edgemagptr, int lowval, int cols)
{
	int *tempmagptr;
	int *tempmapptr;
	int i;
	float thethresh;
	int x[8] = { 1,1,0,-1,-1,-1,0,1 },
		y[8] = { 0,1,1,1,0,-1,-1,-1 };

	for (i = 0; i < 8; i++) {
		tempmapptr = edgemapptr - y[i] * cols + x[i];
		tempmagptr = edgemagptr - y[i] * cols + x[i];

		if ((*tempmapptr == POSSIBLE_EDGE) && (*tempmagptr > lowval)) {
			*tempmapptr = EDGE;
			follow_edges(tempmapptr, tempmagptr, lowval, cols);
		}
	}
}

void Canny::connect_line(int distance) {
	CImg<int> pic = matrix2image(edge);

	bool isEdge[1000][1000];
	int x_neightbor[8] = { 1,1,0,-1,-1,-1,0,1 },
		y_neightbor[8] = { 0,1,1,1,0,-1,-1,-1 };

	// calculating whether a pixel is an edge
	// look at the eight neighborhoods centered on this point. 
	// If only one pixel is near zero, the other are seven-255, then is edge points.
	cimg_forXY(pic, x, y) {
		isEdge[x][y] = false;
		if (x != rows - 1 && x != 0 && y != cols - 1 && y != 0 && pic(x, y) == 0) {
			int neighbors[8];
			int m = 0;
			for (int i = 0; i < 8; i++) {
				int i_neightbor = x + x_neightbor[i];
				int j_neightbor = y + y_neightbor[i];
				if (!(i_neightbor == x && j_neightbor == y)) {
					neighbors[m] = pic(i_neightbor, j_neightbor);
					m++;
				}
			}

			sort(neighbors, neighbors + 8);
			if (neighbors[0] == 0 && neighbors[1] == 255)
				isEdge[x][y] = true;
		}
	}

	cimg_forXY(pic, x, y) {
		if (x >= distance && x <= rows - 1 - distance && y >= distance && y <= cols - 1 - distance && isEdge[x][y] == true) {
			for (int i = x - distance; i <= x + distance; i++) {
				for (int j = y - distance; j <= y + distance; j++) {
					if (isEdge[i][j] == true) {
						pic = Draw_line(pic, x, y, i, j);
						isEdge[i][j] = false;
						isEdge[x][y] = false;
					}
				}
			}
		}
	}

	image2matrix(pic, c_line);
}

CImg<int> Draw_line(CImg<int> tmp, int x ,int y, int x1, int y1) {
	CImg <int> TempImg = tmp;
	int black[] = { 0,0,0 };
	TempImg.draw_line(x, y, x1, y1, black);
	return TempImg;
}

void Canny::delete_line(int distance) {
	CImg<int> pic = matrix2image(c_line);

	bool isEdge[1000][1000];
	int x_neightbor[8] = { 1,1,0,-1,-1,-1,0,1 },
		y_neightbor[8] = { 0,1,1,1,0,-1,-1,-1 };
	
	// calculating whether a pixel is an edge
	// look at the eight neighborhoods centered on this point. 
	// If only one pixel is zero, the other are seven-255, then is edge points.
	cimg_forXY(pic, x, y) {
		isEdge[x][y] = false;
		if (x != rows - 1 && x != 0 && y != cols - 1 && y != 0 && pic(x, y) == 0) {
			int neighbors[8];
			int m = 0;
			for (int i = 0; i < 8; i++) {
				int i_neightbor = x + x_neightbor[i];
				int j_neightbor = y + y_neightbor[i];
				if (!(i_neightbor == x && j_neightbor == y)) {
					neighbors[m] = pic(i_neightbor, j_neightbor);
					m++;
				}
			}

			sort(neighbors, neighbors + 8);
			if (neighbors[0] == 0 && neighbors[1] == 255)
				isEdge[x][y] = true;
			// delete isolate pixel
			if (neighbors[0] == 255)
				pic(x, y) = 255;
		}
	}

	cimg_forXY(pic, x, y) {
		if (isEdge[x][y] == true) {
			int begin_x = x - distance > 0 ? x - distance : 0;
			int begin_y = y - distance > 0 ? y - distance : 0;
			int end_x = x + distance < rows - 1 ? x + distance : rows - 1;
			int end_y = y + distance < cols - 1 ? y + distance : cols - 1;
			for (int i = begin_x; i <= end_x; i++) {
				for (int j = begin_y; j <= end_y; j++) {
					if (isEdge[i][j] == true) {
						int max_x = x >= i ? x : i;
						int max_y = y >= j ? y : j;
						int min_x = max_x == x ? i : x;
						int min_y = max_y == y ? j : y;
						for (int ii = min_x; ii <= max_x; ii++) {
							for (int jj = min_y; jj <= max_y; jj++) {
								pic(ii, jj) = 255;
							}
						}
						isEdge[i][j] = false;
						isEdge[x][y] = false;
					}
				}
			}
		}
	}

	cimg_forXY(pic, x, y) {
		isEdge[x][y] = false;
		if (x != rows - 1 && x != 0 && y != cols - 1 && y != 0 && pic(x, y) == 0) {
			int neighbors[8];
			int m = 0;
			for (int i = 0; i < 8; i++) {
				int i_neightbor = x + x_neightbor[i];
				int j_neightbor = y + y_neightbor[i];
				if (!(i_neightbor == x && j_neightbor == y)) {
					neighbors[m] = pic(i_neightbor, j_neightbor);
					m++;
				}
			}

			sort(neighbors, neighbors + 8);
			// delete isolate pixel
			if (neighbors[0] == 255)
				pic(x, y) = 255;
		}
	}

	image2matrix(pic, d_line);
}


/*******************************************************************
 * test function
 * *****************************************************************/

void Canny::canny_edge_detection(float sigma, float tlow, float thig, int distance) {
	RGB2Gray();
	gaussian_smooth(sigma);
	derrivative_x_y();
	magnitude_x_y();
	radian_direction(-1, -1);
	non_max_supp();
	apply_hysteresis(tlow, thig);
	// CImg<int> d_line_img = matrix2image(edge);
	// d_line_img.display();
	connect_line(distance);
	delete_line(distance);
	save_result();
}

void Canny::canny_ablation_experiment(float sigma, float tlow, float thig, int distance, string savename) {
	string out_name = this->name;

	RGB2Gray();
	gaussian_smooth(sigma);
	derrivative_x_y();
	magnitude_x_y();
	radian_direction(-1, -1);
	non_max_supp();
	apply_hysteresis(tlow, thig);
	connect_line(distance);
	delete_line(distance);

	CImg<int> d_line_img = matrix2image(d_line);
	string outpath = "./ae_ouput/" + savename + out_name;
	d_line_img.save(outpath.c_str());
}

void Canny::save_result() {
	string out_name = this->name;

	string outpath = "./output/gray-" + out_name;
	img.save(outpath.c_str());

	CImg<int> guassian_img = matrix2image(smoothedim);
	outpath = "./output/guassian-" + out_name;
	guassian_img.save(outpath.c_str());

	CImg<int> nms_img = matrix2image(nms);
	outpath = "./output/nms-" + out_name;
	nms_img.save(outpath.c_str());

	CImg<int> edge_img = matrix2image(edge);
	outpath = "./output/edge-" + out_name;
	edge_img.save(outpath.c_str());

	CImg<int> c_line_img = matrix2image(c_line);
	outpath = "./output/cline-" + out_name;
	c_line_img.save(outpath.c_str());

	CImg<int> d_line_img = matrix2image(d_line);
	outpath = "./output/dline-" + out_name;
	d_line_img.save(outpath.c_str());

}


/*******************************************************************
 * axulilary function
 * *****************************************************************/
CImg<int> Canny::matrix2image(int *matrix) {
	CImg<int> picture(rows, cols, 1, 1, 5);
	picture.fill(0);
	cimg_forXY(picture, x, y) {
		picture(x, y, 0) = matrix[x * cols + y];
	}
	return picture;
}

void Canny::image2matrix(CImg<int> img, int *matrix) {
	cimg_forXY(img, x, y) {
		matrix[x * cols + y] = img(x, y, 0);
	}
}


void Canny::show_temp_result(int *matirx) {
	CImg<int> picture(rows, cols, 1, 1, 5);
	picture.fill(0);
	cimg_forXY(img, x, y) {
		picture(x, y, 0) = matirx[x * cols + y];
	}
	// picture.display();
}