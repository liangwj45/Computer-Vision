#include <list>
#include <sstream>
#include <string>
#include "canny.h"
using std::list;
using std::stringstream;

class Test {
 public:
  Test(Canny& img) : img(img) {}
  void test(float sigma, float tlow, float thig, int distance) {
    img.canny_edge_detection(sigma, tlow, thig, distance);
  }
  void save(string tag) { img.save_result(tag); }

 private:
  Canny& img;
};

int main() {
  list<string> images({"3.bmp", "4.bmp", "20160326110137505.bmp", "bigben.bmp",
                       "lena.bmp", "stpietro.bmp"});
  for (auto it = images.begin(); it != images.end(); ++it) {
    Canny img(*it);
    Test test(img);
    test.test(1.5, 0.3, 0.75, 10);
    test.save("");
  }
  return 0;
}
