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
}

void Hough::hough_process(double sigma, double tlow, double thigh, double tgrad,
                          double tvote, double peakdis) {
  edge_detect(sigma, tlow, thigh);
  generate_hough_space(tgrad);
  voting_line(tvote, peakdis);
  get_line_equation();
  draw_lines();
  get_vertexs();
  draw_edge();
  draw_vertexs();
}

void Hough::edge_detect(double sigma, double tlow, double thigh) {
  Canny canny(path.c_str());
  img = canny.canny_edge_detection(sigma, tlow, thigh);
  img.save(output_path("01-edge_detect-" + path));
}

void Hough::generate_hough_space(double tgrad) {
  cout << "generate_hough_space" << endl;
  int maxp = (int)sqrt(img._width * img._width + img._height * img._height);
  hough_space.resize(360, maxp, 1, 1, 0);
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

  cimg_forXY(img, i, j) {
    double grad = sqrt(gradient_x(i, j) * gradient_x(i, j) +
                       gradient_y(i, j) * gradient_y(i, j));
    if (grad > tgrad) {
      cimg_forX(hough_space, alpha) {
        double theta = ((double)alpha * cimg::PI) / 180;
        int p = (int)(i * cos(theta) + j * sin(theta));
        if (p >= 0 && p < maxp) {
          hough_space(alpha, p)++;
        }
      }
    }
  }
  hough_space.save(output_path("02-hough_space-" + path));
}

void Hough::voting_line(double tvote, double peakdis) {
  cout << "voting_line" << endl;
  peaks.clear();
  int x_min = 0, x_max = img._width;
  int y_min = 0, y_max = img._height;
  cimg_forXY(hough_space, theta, p) {
    if (hough_space(theta, p) > tvote) {
      bool flag = true;
      double alpha = (double)theta * cimg::PI / 180;
      const int y0 =
          ((double)p / (sin(alpha))) - double(x_min) * (1 / tan(alpha));
      const int y1 =
          ((double)p / (sin(alpha))) - double(x_max) * (1 / tan(alpha));
      const int x0 = ((double)p / (cos(alpha))) - double(y_min) * (tan(alpha));
      const int x1 = ((double)p / (cos(alpha))) - double(y_max) * (tan(alpha));
      if (x0 >= x_min && x0 <= x_max || x1 >= x_min && x1 <= x_max ||
          y0 >= y_min && y0 <= y_max || y1 >= y_min && y1 <= y_max) {
        for (int i = 0; i < peaks.size(); i++) {
          if ((peaks[i].x - theta) * (peaks[i].x - theta) +
                  (peaks[i].y - p) * (peaks[i].y - p) <
              peakdis * peakdis) {
            flag = false;
            if (peaks[i].value < hough_space(theta, p)) {
              peaks[i] = Point({theta, p, hough_space(theta, p)});
            }
          }
        }
        if (flag) {
          peaks.push_back(Point({theta, p, hough_space(theta, p)}));
        }
      }
    }
  }
  std::sort(peaks.begin(), peaks.end(),
            [](Point a, Point b) { return a.value > b.value; });
}

void Hough::get_line_equation() {
  cout << "get_line_equation" << endl;
  lines.clear();
  for (int i = 0; i < 4; i++) {
    double c = cos(peaks[i].x * cimg::PI / 180);
    double s = sin(peaks[i].x * cimg::PI / 180);
    s = s == 0 ? 0.001 : s;
    lines.push_back(Line({c, s, (double)peaks[i].y}));
    cout << "line " << i + 1 << ": " << peaks[i].y << " = " << -c
         << (s >= 0 ? "x + " : "x ") << s << "y" << endl;
  }
}

void Hough::draw_lines() {
  cout << "draw_lines" << endl;
  static unsigned char blue[] = {0, 0, 255};
  for (int i = 0; i < 4; i++) {
    int xmin = 0, xmax = img._width;
    int yl = (int)((lines[i].c - lines[i].a * xmin) / lines[i].b);
    while (yl < 0 || yl >= img._height) {
      xmin++;
      yl = (int)((lines[i].c - lines[i].a * xmin) / lines[i].b);
    }
    int yr = (int)((lines[i].c - lines[i].a * xmax) / lines[i].b);
    while (yr < 0 || yr >= img._height) {
      xmax--;
      yr = (int)((lines[i].c - lines[i].a * xmax) / lines[i].b);
    }
    img_src.draw_line(xmin, yl, xmax, yr, blue);
  }
  // img_src.save(output_path("03-draw_line-" + path));
}

void Hough::draw_edge() {
  cout << "draw_edge" << endl;
  static unsigned char red[] = {255, 0, 0};
  set<int> xset, yset;
  for (Point vertex : vertexs) {
    xset.insert(vertex.x);
    yset.insert(vertex.y);
  }
  for (int i = 0; i < 4; i++) {
    vector<Point> vs;
    for (int j = 0; j < 4; j++) {
      int diff =
          lines[i].c - lines[i].a * vertexs[j].x - lines[i].b * vertexs[j].y;
      if (abs(diff) < 10) {
        vs.push_back(vertexs[j]);
      }
    }
    if (vs.size() >= 2) {
      img_src.draw_line(vs[0].x, vs[0].y, vs[1].x, vs[1].y, red);
    }
  }
  // img_src.save(output_path("04-draw_edge-" + path));
}

void Hough::get_vertexs() {
  cout << "get_vertexs" << endl;
  vertexs.clear();
  for (int i = 0; i < 4; i++) {
    for (int j = i + 1; j < 4; j++) {
      double a1 = lines[i].a, a2 = lines[j].a;
      double b1 = lines[i].b, b2 = lines[j].b;
      double c1 = lines[i].c, c2 = lines[j].c;
      double x = (c2 * b1 - c1 * b2) / (a2 * b1 - a1 * b2);
      double y = (c2 - a2 * x) / b2;
      printf("(%lf,%lf)\n", x, y);
      if (x >= 0 && x < img._width && y >= 0 && y < img._height) {
        vertexs.push_back(Point({(int)x, (int)y, 0}));
      }
    }
  }
}

void Hough::draw_vertexs() {
  cout << "draw_vertexs" << endl;
  static unsigned char red[] = {255, 0, 0};
  for (int i = 0; i < vertexs.size(); i++) {
    img_src.draw_circle(vertexs[i].x, vertexs[i].y, 5, red);
  }
  img_src.save(output_path("04-lines-" + path));
}
