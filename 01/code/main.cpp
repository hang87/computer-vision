#include "CImg.h"
#include "test.hpp"
using namespace cimg_library;

int main() {
	CImg<unsigned char> image("1.bmp");
	
	Test test(image);
  	test.run();
	
	return 0;
}
