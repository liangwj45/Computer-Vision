#include <iostream>
#include "CImg.h"
#include "HW1.hpp"
using namespace cimg_library;

class Test {
public:
	Test(CImg<unsigned char>& img) : img(img) {}

	void test1() {
		HW1::step1(img);
		img.save("./step1.bmp");
	}

	void test2() {
		CImg<unsigned char> img(this->img);
		HW1::step2(img);
		img.save("./step2.bmp");
	}

	void test3() {
		CImg<unsigned char> img(200, 200, 1, 3);;
		HW1::step3(img);
		img.save("./step3.bmp");
	}

	void test4() {
		CImg<unsigned char> img(200, 200, 1, 3);;
		HW1::step4(img);
		img.save("./step4.bmp");
	}

	void test5() {
		CImg<unsigned char> img(200, 200, 1, 3);;
		HW1::step5(img);
		img.save("./step5.bmp");
	}

	void test6() {
		HW1::step2(img);
		HW1::step3(img);
		HW1::step4(img);
		HW1::step5(img);
		HW1::step6(img);
		img.save("./step6.bmp");
	}

	void runAll() {
		test1();
		test2();
		test3();
		test4();
		test5();
		test6();
	}

private:
	CImg<unsigned char>& img;
};

int main() {
  CImg<unsigned char> img("./1.bmp");
  Test test(img);
  test.runAll();
}
