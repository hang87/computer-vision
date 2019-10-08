  
#include <iostream>
#include "CImg.h"
#include "Canny.h"
#include "HoughForCircle.h"

using namespace std;

int main() {
    HoughForCircle hc;
	hc.setDebug(true);
	hc.hough("data/input/Dataset2/1.bmp");

    return 0;
}