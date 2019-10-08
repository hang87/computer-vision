#pragma once
#ifndef HOUGH_H
#define HOUGH_H

#include <iostream>
#include <string>
#include <vector>
#include "CImg.h"

using namespace std;
using namespace cimg_library;

class Hough {
private:
    CImg<float> image;              // the original image
    CImg<float> outputImage;        // the image of hough transformation
    CImg<float> grayImage;          // the gray image
    CImg<float> thresholdImage;     // threshold proccessed image
    CImg<float> houghImage;         // hough transformation image

    vector<float> setSin;           // sin set
	vector<float> setCos;           // cos set
    vector< vector<float> > filter;   // guassian filter

    int pointNumber;                // num of angle points
    vector< pair<int, int> > lines;   // detected line set
	vector<int> lineWeight;         // line's weight(voting value in hough space) set
    vector<int> sortLineWeight;     // from big to small
	CImg<float> edge;               // edge line

    int circleNumber;
	int minRadius;
	int maxRadius;
    vector<pair<int, int>> circles; 
	vector<pair<int, int>> voteSet; 
	vector<pair<int, int>> center;
	vector<int> circleWeight;
	vector<int> sortCircleWeight;
public:
    Hough(string, string, string, int, int minR = 0, int maxR = 0);

    void RGB2Gray();
    vector< vector<float> > createGuassianFilter(int, int, float);    // create guassian filter
    CImg<float> useFilter(CImg<float>&, vector< vector<float> >&);    // guassian filtering
    CImg<float> sobel(CImg<float>&, CImg<float>&);                  // sobel gradient getting
    CImg<float> nonMaxSupp(CImg<float>&, CImg<float>&);             // non-maximun suppression
    CImg<float> threshold(CImg<float>&, int, int);                  // threshold processing
    void houghLinesTransform(CImg<float>&);                         // hough line transformation
    void houghLinesDetect();                                        // hough line detection
    int getMaxHough(CImg<float>&, int&, int&, int&);                // get the cross point in hough space
    void drawLine(string input, string output);

    void houghCirclesTransform(CImg<float>&, int, int, string);
    void houghCirclesDetect();
    void drawCircle(int, int, string);

    void extraDrawLine();
};

#endif