#include "CImg.h" 
#include "hw1.hpp"

using namespace cimg_library;

class Test {
public:
	Test(CImg<unsigned char>& image) : image(image) {}
	
	void test1() {
		HW1::task1(image);
				
		return;
	}
	
	void test2() {
		CImg<unsigned char> image(this->image);
		HW1::task2(image);
		image.display("task2");
		return;
	}
	
	void test3() {
		CImg<unsigned char> image(400, 400, 1, 3);
		image.fill(0);
		HW1::task3(image);
		image.display("task3");
		return;
	}
	
	void test3_2() {
		CImg<unsigned char> image(400, 400, 1, 3);
		image.fill(0);
		HW1::task3_2(image);
		image.display("task3_2");
		return;
	}
	
	void test4() {
		CImg<unsigned char> image(400, 400, 1, 3);
		image.fill(0);
		HW1::task4(image);
		image.display("task4");
		return;
	}
	
	void test4_2() {
		CImg<unsigned char> image(400, 400, 1, 3);
		image.fill(0);
		HW1::task4_2(image);
		image.display("task4_2");
		return;
	}
	
	void test5() {
		CImg<unsigned char> image(400, 400, 1, 3);
		image.fill(0);
		HW1::task5(image);
		image.display("task5");
		return;
	}
	
	void test5_2() {
		CImg<unsigned char> image(400, 400, 1, 3);
		image.fill(0);
		HW1::task5_2(image);
		image.display("task5_2");
		return;
	}
	
	void test6() {
		HW1::task2(image);
		HW1::task3(image);
		HW1::task4(image);
		HW1::task5(image);
		HW1::task6(image);
		image.display("task6");
	}
	
	void run() {
		test1();
		test2();
		test3();
		test3_2(); 
		test4();
		test4_2();
		test5();
		test5_2();
		test6();
		
		return;
	}
private:
	CImg<unsigned char>& image;	
};
