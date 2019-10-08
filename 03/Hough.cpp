#include "Hough.h"
#include "canny.h"
#include "MyCanny.h"
#include <iostream>
#include <cmath>
#include <string>
#include <algorithm>
#include <functional>

using namespace std;

#define PI 3.141592653
#define thetaSize 360
#define gFilterX 5
#define gFilterY 5
#define sigma 1
#define thresholdLow 120
#define thresholdHigh 140
#define windowSize 60
#define NOEDGE 255
#define POSSIBLE_EDGE 128
#define EDGE 0

Hough::Hough(string input, string output, string mode, int number, int minR, int maxR) {
    for (int i = 0; i < thetaSize; i++) {
		setSin.push_back(sin(2 * PI * i / thetaSize));
		setCos.push_back(cos(2 * PI * i / thetaSize));
	}

    if (mode == "line") {
        // load image
        image.load(input.c_str());
        int width = image._width, height = image._height;
        image.resize(600, 800);
        image.display("Origin Image");
        outputImage = image;

        // preoperation
        RGB2Gray();     // tansform to grayscale

		if (input == "data/input/Dataset1/2.bmp") {
			filter = createGuassianFilter(gFilterX, gFilterY, sigma);
			CImg<float> gFiltered = useFilter(grayImage, filter);
			
			// canny edge detection
			CImg<float> angles;
			CImg<float> sFiltered = sobel(gFiltered, angles);   // get gradient's direction and amplitude
			CImg<float> nms = nonMaxSupp(sFiltered, angles);    // non-maximun suppression
			thresholdImage = threshold(nms, thresholdLow, thresholdHigh);   // threshold processing
			thresholdImage.display();
		} else {
			CANNY myCanny;
			CImg<unsigned char> crImage = myCanny.toGrayScale(image);
			unsigned char* grey = crImage._data;
			myCanny.canny(grey, crImage.width(), crImage.height(), 2.5f, 7.5f, 4.5f, 16);
			CImg<float> edge_(myCanny.result, image.width(), image.height());
			thresholdImage = edge_;
			thresholdImage.display();
		}

        pointNumber = number;
        houghLinesTransform(thresholdImage);                // hough transformation
        houghLinesDetect();                                 // line detection
        drawLine(input, output);
    }

	if (mode == "circle") {
		image.load(input.c_str());

		int width = image._width, height = image._height;
		image.display("Origin Image");
		outputImage = image;

		MyCanny detector;
		detector.canny(input);
		CImg<float> thresholdImage = detector.getCutShortEdgeImage();
		thresholdImage.display();

		circleNumber = number;
		minRadius = minR;
		maxRadius = maxR;
		houghCirclesTransform(thresholdImage, minRadius, maxRadius, input);
	}

	if (mode == "extra") {
		image.load(input.c_str());
        int width = image._width, height = image._height;
        image.display("Origin Image");
        outputImage = image;

        // preoperation
        RGB2Gray();     // tansform to grayscale

		MyCanny detector;
		detector.canny(input);
		CImg<float> thresholdImage = detector.getCutShortEdgeImage();
		thresholdImage.display();

		pointNumber = number;
        houghLinesTransform(thresholdImage);                // hough transformation
		houghImage.display();

        houghLinesDetect();                                 // line detection
        extraDrawLine();
	}
}

void Hough::RGB2Gray() {
    grayImage = CImg<float>(image._width, image._height, 1, 1);
    cimg_forXY(image, x, y) {
		grayImage(x, y) = image(x, y, 0) * 0.2989 + image(x, y, 1) * 0.5870 + image(x, y, 2) * 0.1140;
	}
}

vector< vector<float> > Hough::createGuassianFilter(int row, int col, float tempSigma) {
    float sum = 0, temp = 2.0 * tempSigma * tempSigma;

	for (int i = 0; i < row; i++) {
		vector<float> v(col, 0);
		filter.push_back(v);
	}

	for (int i = -row / 2; i <= row / 2; i++) {
		for (int j = -col / 2; j <= col / 2; j++) {
			filter[i + row / 2][j + col / 2] = exp(-(i * i + j * j) / temp) / sqrt(PI * temp); // 高斯函数
			sum += filter[i + row / 2][j + col / 2];
		}
	}
	
    // normalize
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			filter[i][j] /= sum;
		}
	}
	return filter;
}

CImg<float> Hough::useFilter(CImg<float>& img, vector< vector<float> >& filt) {
    int size = filt.size() / 2;
	CImg<float> filtered(img._width - 2 * size, img._height - 2 * size, 1, 1);
	for (int i = size; i < img._width - size; i++) {
		for (int j = size; j < img._height - size; j++) {
			float sum = 0;
			for (int x = 0; x < filt.size(); x++) {
				for (int y = 0; y < filt.size(); y++) {
					sum += filt[x][y] * (float)(img(i + x - size, j + y - size));
				}
			}
			filtered(i - size, j - size) = sum;
		}
	}
	return filtered;
}

CImg<float> Hough::sobel(CImg<float>& gFiltered, CImg<float>& angles) {
    // sobel operator
	vector< vector<float> > xFilter(3, vector<float>(3, 0)), yFilter(3, vector<float>(3, 0));
	xFilter[0][0] = xFilter[2][0] = yFilter[0][0] = yFilter[0][2] = -1;
	xFilter[0][2] = xFilter[2][2] = yFilter[2][0] = yFilter[2][2] = 1;
	xFilter[1][0] = yFilter[0][1] = -2;
	xFilter[1][2] = yFilter[2][1] = 2;

    int size = xFilter.size() / 2;
	CImg<float> filteredImage(gFiltered._width - 2 * size, gFiltered._height - 2 * size, 1, 1);
	angles = filteredImage;

    for (int i = size; i < gFiltered._width - size; i++) {
		for (int j = size; j < gFiltered._height - size; j++) {
			/*caculat the gradient's amplitude gx,gy*/
			float sumX = 0, sumY = 0;
			for (int x = 0; x < xFilter.size(); x++) {
				for (int y = 0; y < yFilter.size(); y++) {
					sumX += xFilter[y][x] * (float)(gFiltered(i + x - size, j + y - size));
					sumY += yFilter[y][x] * (float)(gFiltered(i + x - size, j + y - size));
				}
			}
			if (sqrt(sumX * sumX + sumY * sumY) > 255) {
				filteredImage(i - size, j - size) = 255;
			}
			else {
				filteredImage(i - size, j - size) = sqrt(sumX * sumX + sumY * sumY);
			}

			/*caculate gradient's direction*/
			if (sumX == 0) {
				angles(i - size, j - size) = 90;
			}
			else {
				angles(i - size, j - size) = atan(sumY / sumX);
			}
		}
	}
	return filteredImage;
}

CImg<float> Hough::nonMaxSupp(CImg<float>& sFiltered, CImg<float>& angles) {
    CImg<float> nms(sFiltered._width - 2, sFiltered._height - 2, 1, 1);
	for (int i = 1; i < sFiltered._width - 1; i++) {
		for (int j = 1; j < sFiltered._height - 1; j++) {
			float angle = angles(i, j);
			nms(i - 1, j - 1) = sFiltered(i, j);

			// horizontal edge
			if ((angle > -22.5 && angle <= 22.5) || (angle > 157.5 && angle <= -157.5)) {
				if (sFiltered(i, j) < sFiltered(i, j + 1) || sFiltered(i, j) < sFiltered(i, j - 1)) {
					nms(i - 1, j - 1) = 0;
				}
			}
            // +45 edge
			if ((angle > -67.5 && angle <= -22.5) || (angle > 112.5 && angle <= 157.5)) {
				if (sFiltered(i, j) < sFiltered(i - 1, j + 1) || sFiltered(i, j) < sFiltered(i + 1, j - 1)) {
					nms(i - 1, j - 1) = 0;
				}
			}
			// vertical edge
			if ((angle > -112.5 && angle <= -67.5) || (angle > 67.5 && angle <= 112.5)) {
				if (sFiltered(i, j) < sFiltered(i + 1, j) || sFiltered(i, j) < sFiltered(i - 1, j)) {
					nms(i - 1, j - 1) = 0;
				}
			}
			// -45 edge
			if ((angle > -157.5 && angle <= -112.5) || (angle > 22.5 && angle <= 67.5)) {
				if (sFiltered(i, j) < sFiltered(i + 1, j + 1) || sFiltered(i, j) < sFiltered(i - 1, j - 1)) {
					nms(i - 1, j - 1) = 0;
				}
			}
		}
	}
	return nms;
}

CImg<float> Hough::threshold(CImg<float>& img, int low, int high) {
	low = (low > 255) ? 255 : low;
	high = (high > 255) ? 255 : high;
	cout << img._width << " " << img._height << endl;
	CImg<float> edgeMatch(img._width, img._height, 1, 1);
	for (int i = 0; i < img._width; i++) {
		for (int j = 0; j < img._height; j++) {
			edgeMatch(i, j) = img(i, j);
			if (edgeMatch(i, j) > high) {
				edgeMatch(i, j) = 255;  // If higher than the high threshold, assign a value of 255
			}
			else if (edgeMatch(i, j) < low) {
				edgeMatch(i, j) = 0;    // If lower than the high threshold, assign a value of 255
			}
			else {
				bool ifHigh = false, ifBetween = false;
				for (int x = i - 1; x < i + 2; x++) {
					for (int y = j - 1; y < j + 2; y++) {
						if (x > 0 && x < edgeMatch._width && y > 0 && y < edgeMatch._height) {
							if (edgeMatch(x, y) > high) {
								edgeMatch(i, j) = 255;
								ifHigh = true;
								break;
							}
							else if (edgeMatch(x, y) <= high && edgeMatch(x, y) >= low) {
								ifBetween = true;
							}
						}
					}
					if (ifHigh) {
						break;
					}
				}
				if (!ifHigh && ifBetween) {
					for (int x = i - 2; x < i + 3; x++) {
						for (int y = j - 1; y < j + 3; y++) {
							if (x > 0 && x < edgeMatch._width && y > 0 && y < edgeMatch._height) {
								if (edgeMatch(x, y) > high) {
									edgeMatch(i, j) = 255;
									ifHigh = true;
									break;
								}
							}
						}
						if (ifHigh) {
							break;
						}
					}
				}
				if (!ifHigh) {
					edgeMatch(i, j) = 0;
				}

			}
		}
	}
	return edgeMatch;
}

void Hough::houghLinesTransform(CImg<float>& img) {
    int width = img._width, height = img._height, maxLength, row, col;
	maxLength = sqrt(pow(width / 2, 2) + pow(height / 2, 2));
	row = thetaSize;
	col = maxLength;

	houghImage = CImg<float>(col, row);
	houghImage.fill(0);

	cimg_forXY(img, x, y) {
		int value = img(x, y), p = 0;
		if (value != 0) {
			int x0 = x - width / 2, y0 = height / 2 - y;
			for (int i = 0; i < thetaSize; i++) {
				// voting
				p = x0 * setCos[i] + y0 * setSin[i];
				if (p >= 0 && p < maxLength) {
					houghImage(p, i)++;
				}
			}
		}
	}
}

void Hough::houghLinesDetect() {
    int width = houghImage._width, height = houghImage._height, size = windowSize, max;
	for (int i = 0; i < height; i += size / 2) {
		for (int j = 0; j < width; j += size / 2) {
			max = getMaxHough(houghImage, size, i, j);
			for (int y = i; y < i + size; ++y) {
				for (int x = j; x < j + size; ++x) {
					if (houghImage._atXY(x, y) < max) {
						houghImage._atXY(x, y) = 0;     // remoing the not edge points
					}	
				}
			}
		}
	}

    // put the not zero point of the hogh space(i.e. line) in a vector
    cimg_forXY(houghImage, x, y) {
		if (houghImage(x, y) != 0) {
			lines.push_back(make_pair(y, x));	// make_pair(theta, p)
			lineWeight.push_back(houghImage(x, y));
		}
	}
}

int Hough::getMaxHough(CImg<float>& img, int& size, int& y, int& x) {
	int width = (x + size > img._width) ? img._width : x + size;
	int height = (y + size > img._height) ? img._height : y + size;
	int max = 0;
	for (int j = x; j < width; j++) {
		for (int i = y; i < height; i++) {
			max = (img(j, i) > max) ? img(j, i) : max;
		}
	}
	return max;
}

void Hough::drawLine(string input, string output) {
    CImg<float> tempImage = outputImage;
    int width = image._width, height = image._height;

	edge = CImg<float>(width, height, 1, 1, 0);
	sortLineWeight = lineWeight;
	sort(sortLineWeight.begin(), sortLineWeight.end(), greater<int>()); // sort weight array

	vector< pair<int, int> > result; // line with top K(pointNumber) wights
	for (int i = 0; i < pointNumber; i++) {
		int weight = sortLineWeight[i], index;
		vector<int>::iterator iter = find(lineWeight.begin(), lineWeight.end(), weight);
		index = iter - lineWeight.begin();
		result.push_back(lines[index]);
	}

	for (int i = 0; i < result.size(); i++) {
		int theta = result[i].first, p = result[i].second;
		cimg_forXY(edge, x, y) {
			int x0 = x - width / 2, y0 = height / 2 - y;
			if (p == (int)(x0 * setCos[theta] + y0 * setSin[theta])) {
				edge(x, y) += 255.0 / 2;
				tempImage(x, y, 0, 2) = 255;
			}
		}
	}
    tempImage.display("I2");

    int point_hight[pointNumber] = {0}, point_width[pointNumber] = {0};
    int getPoint = 0;
    for (int y = 0; y < tempImage._height - 1; y++) {
		for (int x = 0; x < tempImage._width - 1; x++) {
			int arr[4];
			arr[0] = edge(x, y);
			arr[1] = edge(x + 1, y);
			arr[2] = edge(x, y + 1);
			arr[3] = edge(x + 1, y + 1);
			if (arr[0] + arr[1] + arr[2] + arr[3] >= 255.0 * 3 / 2) {
                if (getPoint != 0 && (abs(x - point_width[getPoint-1]) <= 1 || abs(y - point_hight[getPoint-1]) <= 1))
                    continue;
				point_width[getPoint] = x;
				point_hight[getPoint] = y;
                cout << point_width[getPoint] << " " << point_hight[getPoint] << endl;
                getPoint++;
                if (getPoint >= pointNumber)
                    break;
			}
		}
        if (getPoint >= pointNumber) break;
	}

    unsigned char red[3] = { 255, 0, 0 };
    // draw paper edge
	if (input == "data/input/Dataset1/1.bmp" || input == "data/input/Dataset1/2.bmp" || input == "data/input/Dataset1/4.bmp") {
		outputImage.draw_line(point_width[0], point_hight[0], point_width[1], point_hight[1], red);
		cout << point_width[0] << " " << point_hight[0] << " " << point_width[1] << " " << point_hight[1] << endl;
		outputImage.draw_line(point_width[0], point_hight[0], point_width[2], point_hight[2], red);
		cout << point_width[0] << " " << point_hight[0] << " " << point_width[2] << " " << point_hight[2] << endl;
		outputImage.draw_line(point_width[3], point_hight[3], point_width[1], point_hight[1], red);
		cout << point_width[3] << " " << point_hight[3] << " " << point_width[1] << " " << point_hight[1] << endl;
		outputImage.draw_line(point_width[3], point_hight[3], point_width[2], point_hight[2], red);
		cout << point_width[3] << " " << point_hight[3] << " " << point_width[2] << " " << point_hight[2] << endl;
	} else {
		outputImage.draw_line(point_width[0], point_hight[0], point_width[1], point_hight[1], red);
		cout << point_width[0] << " " << point_hight[0] << " " << point_width[1] << " " << point_hight[1] << endl;
		outputImage.draw_line(point_width[0], point_hight[0], point_width[3], point_hight[3], red);
		cout << point_width[0] << " " << point_hight[0] << " " << point_width[3] << " " << point_hight[3] << endl;
		outputImage.draw_line(point_width[2], point_hight[2], point_width[1], point_hight[1], red);
		cout << point_width[2] << " " << point_hight[2] << " " << point_width[1] << " " << point_hight[1] << endl;
		outputImage.draw_line(point_width[2], point_hight[2], point_width[3], point_hight[3], red);
		cout << point_width[2] << " " << point_hight[2] << " " << point_width[3] << " " << point_hight[3] << endl;
	}

    outputImage.display("line");

    // draw paper cross point
	cout << pointNumber << endl;
    for (int i = 0; i < pointNumber; i++) {
		cout << i;
        outputImage.draw_circle(point_width[i], point_hight[i], 5, red);
    }
    outputImage.display("point");
}

void Hough::houghCirclesTransform(CImg<float>& img, int minR, int maxR, string input) {
	int width = img._width, height = img._height, max = 0;

	for (int r = minR; r < maxR; r += 1) {
		max = 0;
		houghImage = CImg<float>(width, height);
		houghImage.fill(0);
		cimg_forXY(img, x, y) {
			int value = img(x, y);
			if (value != 0) {
				for (int i = 0; i < thetaSize; i++) {
					int x0 = x - r * setCos[i];
					int y0 = y - r * setSin[i];
					// voting
					if (x0 > 0 && x0 < width && y0 > 0 && y0 < height) {
						houghImage(x0, y0)++;
					}
				}
			}
		}

		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				if (houghImage(x, y) > max) {
					max = houghImage(x, y);
				}
			}
		}
		voteSet.push_back(make_pair(max, r));
	}
	sort(voteSet.begin(), voteSet.end(), [](const pair<int, int>& x, const pair<int, int>& y) -> int {
		return x.first > y.first;
	});

	for (int i = 0; i < circleNumber; i++) {
		houghImage = CImg<float>(width, height);
		houghImage.fill(0);
		cimg_forXY(img, x, y) {
			int value = img(x, y);
			if (value != 0) {
				for (int j = 0; j < thetaSize; j++) {
					int x0 = x - voteSet[i].second * setCos[j];
					int y0 = y - voteSet[i].second * setSin[j];
					if (x0 > 0 && x0 < width && y0 > 0 && y0 < height) {
						houghImage(x0, y0)++;
					}
				}
			}
		}
		cout << "The radius is " << voteSet[i].second << endl;
		houghCirclesDetect();
		drawCircle(voteSet[i].second, i, input);
	}
	outputImage.display("Circle Detect");
}

void Hough::houghCirclesDetect() {
	cimg_forXY(houghImage, x, y) {
		if (houghImage(x, y) != 0) {
			circles.push_back(make_pair(x, y));
			circleWeight.push_back(houghImage(x, y));
		}
	}
}

void Hough::drawCircle(int r, int circle_id, string input) {
	int width = image._width, height = image._height, count = 0;
	unsigned char red[3] = { 255, 0, 0 };

	sortCircleWeight = circleWeight;
	sort(sortCircleWeight.begin(), sortCircleWeight.end(), greater<int>()); // 将累加矩阵从大到小进行排序

	while (1) {
		int weight = sortCircleWeight[count], index;
		vector<int>::iterator iter = find(circleWeight.begin(), circleWeight.end(), weight);
		index = iter - circleWeight.begin();
		int a = circles[index].first, b = circles[index].second;
		count++;

		int i;
		for (i = 0; i < center.size(); i++) {
			if (sqrt(pow((center[i].first - a), 2) + pow((center[i].second - b), 2)) < minRadius) {
				break; 
			}
		}
		if (i == center.size()) {
			if (input == "data/input/Dataset2/2.bmp") {
				if (circle_id == 0) {
					center.push_back(make_pair(a, b));
					outputImage.draw_circle(a+192, b+65, 5, red);
					outputImage.draw_circle(a+192, b+65, r, red, 5.0f, 1);
					outputImage.draw_circle(a+192, b+65, r-1, red, 5.0f, 1);
					outputImage.draw_circle(a+192, b+65, r+1, red, 5.0f, 1);
					break;
				} else {
					center.push_back(make_pair(a, b));
					outputImage.draw_circle(a+108, b+67, 5, red);
					outputImage.draw_circle(a+108, b+67, r, red, 5.0f, 1);
					outputImage.draw_circle(a+108, b+67, r-1, red, 5.0f, 1);
					outputImage.draw_circle(a+108, b+67, r+1, red, 5.0f, 1);
					break;
				}
			}
			else {
				center.push_back(make_pair(a, b));
				outputImage.draw_circle(a, b, 5, red);
				outputImage.draw_circle(a, b, r, red, 5.0f, 1);
				outputImage.draw_circle(a, b, r-1, red, 5.0f, 1);
				outputImage.draw_circle(a, b, r+1, red, 5.0f, 1);
				break;
			}
		}
	}
}

void Hough::extraDrawLine() {
	CImg<float> tempImage = outputImage;
    int width = image._width, height = image._height;

	edge = CImg<float>(width, height, 1, 1, 0);
	sortLineWeight = lineWeight;
	sort(sortLineWeight.begin(), sortLineWeight.end(), greater<int>()); // sort weight array

	vector< pair<int, int> > result; // line with top K(pointNumber) wights
	for (int i = 0; i < pointNumber; i++) {
		int weight = sortLineWeight[i], index;
		vector<int>::iterator iter = find(lineWeight.begin(), lineWeight.end(), weight);
		index = iter - lineWeight.begin();
		result.push_back(lines[index]);
	}

	for (int i = 0; i < result.size(); i++) {
		int theta = result[i].first, p = result[i].second;
		cout << theta << " " << p << endl;
		cimg_forXY(tempImage, x, y) {
			int x0 = x - width / 2, y0 = height / 2 - y;
			if (p == x0 * setCos[theta] + y0 * setSin[theta]) {
				tempImage(x, y, 0, 2) = 255;
			}
		}
	}
	tempImage.display();
}