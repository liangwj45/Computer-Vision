#include "Hough.h"
#include <algorithm>
#include <iostream>
#include <set>
using std::cout;
using std::endl;
using std::set;

#define output_path(file) ("./result/" + (file)).c_str()
#define src_path(file) ("img/" + (file)).c_str()

Hough::Hough(string path) : path(path) {
  img_src = CImg<unsigned char>(src_path(path));
  for (int i = 0; i < 360; i++) {
    sin_set.push_back(sin(cimg::PI * i / 180));
    cos_set.push_back(cos(cimg::PI * i / 180));
  }
}

void Hough::edge_detect(double sigma, double tlow, double thigh) {
  Canny canny(path.c_str());
  img = canny.canny_edge_detection(sigma, tlow, thigh);
  img.save(output_path("01-edge_detect-" + path));
}

void Hough::hough_process(double sigma, double tlow, double thigh, double tgrad,
                          int minR, int maxR, double tvote, int center_dis) {
  edge_detect(sigma, tlow, thigh);
  vote_centers(tgrad, tvote, minR, maxR, center_dis);
  draw_circles();
  return;
}

void Hough::vote_centers(double tgrad, double tvote, int minR, int maxR,
                         int center_dis) {
  cout << "vote_centers" << endl;
  int maxp = (int)sqrt(img._width * img._width + img._height * img._height);
  CImg<unsigned char> sobelx(3, 3, 1, 1, 0);
  CImg<unsigned char> sobely(3, 3, 1, 1, 0);
  sobelx(0, 0) = -1, sobely(0, 0) = 1;
  sobelx(0, 1) = 0, sobely(0, 1) = 2;
  sobelx(0, 2) = 1, sobely(0, 2) = 1;
  sobelx(1, 0) = -2, sobely(1, 0) = 0;
  sobelx(1, 1) = 0, sobely(1, 1) = 0;
  sobelx(1, 2) = 2, sobely(1, 2) = 0;
  sobelx(2, 0) = -1, sobely(2, 0) = -1;
  sobelx(2, 1) = 0, sobely(2, 1) = -2;
  sobelx(2, 2) = 1, sobely(2, 2) = -1;
  CImg<unsigned char> gradient_x = img;
  gradient_x = gradient_x.get_convolve(sobelx);
  CImg<unsigned char> gradient_y = img;
  gradient_y = gradient_y.get_convolve(sobely);

  vector<Circle> vps;
  for (int r = minR; r < maxR; r++) {
    hough_space = CImg<unsigned char>(img._width, img._height);
    hough_space.fill(0);
    cimg_forXY(img, x, y) {
      double grad = sqrt(gradient_x(x, y) * gradient_x(x, y) +
                         gradient_y(x, y) * gradient_y(x, y));
      if (grad > tgrad) {
        for (int i = 0; i < 360; i++) {
          int x0 = x - r * cos_set[i];
          int y0 = y - r * sin_set[i];
          if (x0 > 0 && x0 < img._width && y0 > 0 && y0 < img._height) {
            hough_space(x0, y0)++;
          }
        }
      }
    }
    cimg_forXY(hough_space, x, y) {
      if (hough_space(x, y) > tvote) {
        vps.push_back(Circle({x, y, r, hough_space(x, y)}));
      }
    }
  }
  std::sort(vps.begin(), vps.end(),
            [](Circle a, Circle b) { return a.value > b.value; });

  circles.clear();
  for (int i = 0; i < vps.size(); i++) {
    bool flag = true;
    for (int j = 0; j < circles.size(); j++) {
      int dx = vps[i].x - circles[j].x;
      int dy = vps[i].y - circles[j].y;
      if (dx * dx + dy * dy < center_dis * center_dis) {
        flag = false;
        break;
      }
    }
    if (flag) {
      circles.push_back(vps[i]);
    }
  }
}

void Hough::draw_circles() {
  cout << "draw_circles" << endl;
  static unsigned char blue[] = {0, 0, 255};
  static unsigned char red[] = {255, 0, 0};
  for (auto c : circles) {
    img_src.draw_circle(c.x, c.y, 2, red);
    // img_src.draw_circle(c.x, c.y, c.r, blue);
    img_src.draw_circle(c.x, c.y, c.r, blue, 5.0f, 5);
  }
  img_src.save(output_path("02-draw_circles-" + path));
}
