#include "Ex5.h"
#include <stdio.h>
#include <algorithm>
#include <iostream>
using std::cout;
using std::endl;

int Ex5::dir[8][2] = {-1, 0, 1, 0, 0, -1, 0, 1, -1, -1, -1, 1, 1, -1, 1, 1};
unsigned char Ex5::red[3] = {255, 0, 0};
unsigned char Ex5::blue[3] = {0, 0, 255};

#define InImage(x, y) ((x) >= 0 && (x) < width && (y) >= 0 && (y) < height)

Ex5::Ex5(string path) {
  src = img = CImg<unsigned char>(path.c_str());
  img.resize(img._width, img._height, 1, 1);
  width = img._width, height = img._height;
  Devision();
  vis.resize(width, height, 1, 1);
}

void Ex5::test() {
  Erosion(img, img, 3);
  img.save("./img/2-erosion.bmp");
  FindScaleplate(src);
  tcon = 300, tnum = 80;
  EliminateConnection();
  img.save("./img/3-eliminate_connection.bmp");
  src.save("./img/4-draw_frame.bmp");
  DrawScaleplate();
  src.save("./img/5-draw_scaleplate.bmp");
}

void Ex5::Erosion(CImg<unsigned char> in, CImg<unsigned char>& out, int n) {
  cout << "Erosion" << endl;
  cimg_forXY(out, i, j) {
    bool flag = false;
    for (int x = -n / 2; x <= n / 2; x++) {
      if (flag) break;
      for (int y = -n / 2; y <= n / 2; y++) {
        int xx = i + x, yy = j + y;
        if (InImage(xx, yy) && !in(xx, yy)) {
          flag = true;
          break;
        }
      }
    }
    out(i, j) = flag ? 0 : 255;
  }
}

void Ex5::Dilation(CImg<unsigned char> in, CImg<unsigned char>& out, int n) {
  cout << "Dilation" << endl;
  cimg_forXY(out, i, j) {
    bool flag = false;
    for (int x = -n / 2; x <= n / 2; x++) {
      if (flag) break;
      for (int y = -n / 2; y <= n / 2; y++) {
        int xx = i + x, yy = j + y;
        if (InImage(xx, yy) && in(xx, yy)) {
          flag = true;
          break;
        }
      }
    }
    out(i, j) = flag ? 255 : 0;
  }
}

void Ex5::EliminateConnection() {
  cout << "EliminateConnection" << endl;
  cimg_forXY(vis, i, j) { vis(i, j) = 0; }
  cimg_forXY(img, i, j) {
    if (!img(i, j) && !vis(i, j)) {
      cnt = 0, lx = rx = i, ly = ry = j;
      FindConnection(i, j);
      if (cnt > tcon || cnt < tnum) {
        DeleteConnection();
      } else {
        lx -= 2, ly -= 2, rx += 2, ry += 2;
        src.draw_line(lx, ly, rx, ly, red);
        src.draw_line(lx, ly, lx, ry, red);
        src.draw_line(rx, ly, rx, ry, red);
        src.draw_line(lx, ry, rx, ry, red);
      }
    }
  }
}

void Ex5::FindConnection(int x, int y) {
  vis(x, y) = 1;
  con[cnt][0] = x, con[cnt][1] = y;
  if (x < lx) lx = x;
  if (y < ly) ly = y;
  if (x > rx) rx = x;
  if (y > ry) ry = y;
  for (int i = 0; i < 8; i++) {
    int xx = dir[i][0] + x;
    int yy = dir[i][1] + y;
    if (InImage(xx, yy) && !vis(xx, yy) && !img(xx, yy)) {
      ++cnt;
      FindConnection(xx, yy);
    }
  }
}

void Ex5::DeleteConnection() {
  for (int i = 0; i < cnt; i++) {
    img(con[i][0], con[i][1]) = 255;
  }
}

void Ex5::FindScaleplate(CImg<unsigned char>& img) {
  cout << "FindScaleplate" << endl;
  int max = 0, cnt, lx, rx;
  for (int i = 0; i < height; i++) {
    cnt = 0, lx = width, rx = 0;
    for (int j = 0; j < width; j++) {
      if (!img(j, i)) {
        cnt++;
        lx = std::min(lx, j);
        rx = std::max(rx, j);
      }
    }
    if (cnt > max) {
      max = cnt, bx = lx, ex = rx, by = i;
    }
  }
  int suby = 20, addy = 55, dx = 15;
  ey = by + addy, bx -= dx, ex += dx, by -= suby;
}

void Ex5::DrawScaleplate() {
  cout << "DrawScaleplate" << endl;
  DrawLine(src, bx, by, ex, ey, blue, 5);
  DrawLine(src, bx, ey, ex, ey, blue, 5);
  DrawLine(src, bx, by, bx, ey, blue, 5);
  DrawLine(src, ex, by, ex, ey, blue, 5);
}

void Ex5::Devision() {
  cout << "Devision" << endl;
  // int cnt[256];
  // for (int i = 0; i < 256; i++) cnt[i] = 0;
  // cimg_forXY(img, x, y) { cnt[img(x, y, 0)]++; }
  // int min = cnt[0];
  // for (int i = 1; i < 256; i++) min = std::min(cnt[i], min);
  int min = 250;
  cimg_forXY(img, x, y) { img(x, y) = img(x, y) < min ? 0 : 255; }
}

void Ex5::DrawLine(CImg<unsigned char>& img, int bx, int by, int ex, int ey,
                   unsigned char* color, int n) {
  if (bx == ex) {
    for (int i = -n / 2; i < n / 2; i++) {
      img.draw_line(bx + i, by, bx + i, ey, color);
    }
  } else {
    for (int i = -n / 2; i < n / 2; i++) {
      img.draw_line(bx, by + i, ex, by + i, color);
    }
  }
}