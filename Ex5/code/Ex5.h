#ifndef _Ex5_
#define _Ex5_
#include <list>
#include <map>
#include <string>
#include "CImg.h"
using std::list;
using std::map;
using std::string;
using namespace cimg_library;

class Ex5 {
 public:
  Ex5(string path);
  void test();

  struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
  };

  struct Rectangle {
    int lx, rx, ly, ry;
  };

 private:           // Part1
  void Devision();  // 图像分割

 private:  // Part2
  void Erosion(CImg<unsigned char> in, CImg<unsigned char>& out,
               int n);  // 腐蚀
  void Dilation(CImg<unsigned char> in, CImg<unsigned char>& out,
                int n);               // 膨胀
  void EliminateConnection();         // 消除连通块
  void FindConnection(int x, int y);  // 寻找连通块
  void DeleteConnection();            // 删除连通块
  void DrawLine(CImg<unsigned char>& img, int bx, int by, int ex, int ey,
                unsigned char* color, int n);  // 画线条

 private:                                         // Part3
  void FindScaleplate(CImg<unsigned char>& img);  // 找刻度尺
  void DrawScaleplate();                          // 画刻度尺

 private:
  CImg<unsigned char> src;  // 原图
  CImg<unsigned char> img;
  CImg<unsigned char> tmp;
  int width;             // 图像宽度
  int height;            // 图像高度
  int cnt;               // 记录连通块中像素个数
  int lx, rx, ly, ry;    // 记录连通块的边界
  int bx, ex, by, ey;    // 记录刻度尺的边界
  int con[5000][2];      // 记录连通块位置
  CImg<bool> vis;        // 记录是否已经遍历
  static int dir[8][2];  // 八个移动方向
  static unsigned char red[3];
  static unsigned char blue[3];
  int tcon;
  int tnum;
};

#endif