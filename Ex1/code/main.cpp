#include <iostream>
#include "CImg.h"
#include "MyCImg.h"
using namespace cimg_library;

static const unsigned char white[3] = {255, 255, 255};
static const unsigned char black[3] = {0, 0, 0};
static const unsigned char red[3] = {255, 0, 0};
static const unsigned char green[3] = {0, 255, 0};
static const unsigned char blue[3] = {0, 0, 255};
static const unsigned char yellow[3] = {255, 255, 0};
static const double pi(3.14159265);

class Test {
 public:
  Test(MyCImg& myCImg) : myCImg(myCImg) {}

  void test1() {
    myCImg.display();
    myCImg.save("./step1.bmp");
  }

  void test2() {
    myCImg.changeColor(white, red).changeColor(black, green);
    myCImg.save("./step2.bmp");
  }

  void test3() {
    CImg<unsigned char> img(200, 200, 1, 3);
    MyCImg myCImg(img);
    myCImg.drawRegularTriangle(50, 50, 40, blue);
    myCImg.save("./step3-myCImg.bmp");

    CImg<unsigned char> img2(200, 200, 1, 3);
    const int x0 = 30;
    const int y0 = 50 - (int)(20 * sqrt(3) / 3.0);
    const int x1 = 70;
    const int y1 = y0;
    const int x2 = 50;
    const int y2 = 50 + (int)(20 * sqrt(3) / 3.0 * 2);
    img2.draw_triangle(x0, y0, x1, y1, x2, y2, blue);
    img2.save("./step3-CImg.bmp");
  }

  void test4() {
    CImg<unsigned char> img(200, 200, 1, 3);
    MyCImg myCImg(img);
    myCImg.drawCircle(50, 50, 15, yellow);
    myCImg.save("./step4-myCImg.bmp");

    CImg<unsigned char> img2(200, 200, 1, 3);
    img2.draw_circle(50, 50, 15, yellow);
    img2.save("./step4-CImg.bmp");
  }

  void test5() {
    CImg<unsigned char> img(200, 200, 1, 3);
    MyCImg myCImg(img);
    myCImg.drawLine(0, 0, 1.0, 100, green);
    myCImg.save("./step5-myCImg.bmp");

    CImg<unsigned char> img2(200, 200, 1, 3);
    img2.draw_line(0, 0, (int)(100.0 * cos(45.0 * pi / 180.0)), (int)(100.0 * sin(45.0 * pi / 180.0)),
                   green);
    img2.save("./step5-CImg.bmp");
  }

  void test6() {
    myCImg.drawRegularTriangle(50, 50, 40, blue);
    myCImg.drawCircle(50, 50, 15, yellow);
    myCImg.drawLine(0, 0, 1.0, 100, green);
    myCImg.save("./step6.bmp");
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
  MyCImg& myCImg;
};

int main() {
  CImg<unsigned char> img("./1.bmp");
  MyCImg myCImg(img);
  Test test(myCImg);
  test.runAll();
}
