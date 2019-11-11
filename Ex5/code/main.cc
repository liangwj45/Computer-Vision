#include <iostream>
#include "Ex5.h"
#include "Identify.h"
using std::cout;
using std::endl;

int main() {
  cout << "img1" << endl;
  Ex5 seg1("./img/1.bmp", "1");
  seg1.test(300, 80, 0);
  cout << "img2" << endl;
  Ex5 seg2("./img/2.bmp", "2");
  seg2.test(300, 80, 0);
  // cout << "33333" << endl;
  // Ex5 seg3("./img/3.bmp", "3");
  // seg3.test(300, 80, 140);
  // Identify id(true);
  // for (int i = 1; i < 10; i++) {
  //  std::cout << id.Predict("result/" + std::to_string(i) + ".bmp")
  //            << std::endl;
  //}
}
