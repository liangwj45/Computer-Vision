#ifndef _Ex5_
#define _Ex5_
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>
#include "CImg.h"
#include "Identify.h"
using std::list;
using std::map;
using std::set;
using std::string;
using std::vector;
using namespace cimg_library;

class Ex5 {
 public:
  Ex5(string path, string name);
  ~Ex5();
  void test(int tcon, int tnum, int tdiv);

  struct Rectangle {
    int lx, ly, rx, ry, width, height, area;
    Rectangle(int lx = 0, int ly = 0, int rx = 0, int ry = 0, int width = 0,
              int height = 0, int area = 0);
  };

  struct Bracket {
    int lx, rx, y, mid;
    float value;
    Bracket(int lx = 0, int rx = 0, int y = 0, int mid = 0);
  };

  struct Num {
    Rectangle rec;
    vector<Rectangle> nums;
    Num() {}
  };

 private:
  void Devision(int tdiv);                        // 图像分割
  void EliminateConnection(int tcon, int tnum);   // 消除连通块
  void FindConnection(int x, int y);              // 寻找连通块
  void DeleteConnection();                        // 删除连通块
  void FindScaleplate(CImg<unsigned char>& img);  // 找刻度尺
  void DrawScaleplate();                          // 画刻度尺
  void Erosion(CImg<unsigned char> in, CImg<unsigned char>& out,
               int n);  // 腐蚀
  void Dilation(CImg<unsigned char> in, CImg<unsigned char>& out,
                int n);  // 膨胀
  void DrawLine(CImg<unsigned char>& img, int bx, int by, int ex, int ey,
                unsigned char* color, int n);  // 画线条
  void SubImage(const Rectangle& rec, CImg<unsigned char>& out);
  void DrawRectangle(const Rectangle& rec, unsigned char* color = red);
  void DrawRectangle(int lx, int ly, int rx, int ry,
                     unsigned char* color = red);
  void FindNumberAreas();
  void Summary();
  void IdentifyNumber();
  void FindBracket();
  void ComputeCoordinate();
  float Calculate(int x);

 private:
  CImg<unsigned char> img;
  CImg<unsigned char> src;  // 原图
  CImg<unsigned char> div;
  CImg<unsigned char> result;  // 结果图
  string name;                 // 原图名称
  int width, height;           // 图像宽度和高度
  int lx, rx, ly, ry;          // 记录连通块的边界
  int bx, ex, by, ey;          // 记录刻度尺的边界
  int cbx, cex;
  int clx, crx, cy;
  bool* vis;              // 记录是否已经遍历
  int* con;               // 记录连通块像素位置
  vector<Rectangle> rec;  // 记录连通块矩形位置
  int cnt_con;            // 记录连通块中像素个数
  int cnt_rec;            // 记录连通块矩形的个数
  vector<std::pair<int, int>> cnt_area;
  set<int> set_area;
  Rectangle up, down;
  map<int, vector<Rectangle>> up_rec;
  map<int, vector<Rectangle>> down_rec;
  map<int, vector<Rectangle>> ruler_rec;
  vector<Bracket> brackets;
  vector<Rectangle> bracket_nums;
  Identify identify;

 private:
  static int dir[8][2];           // 八个移动方向
  static unsigned char red[3];    // 红色像素值
  static unsigned char blue[3];   // 蓝色像素值
  static unsigned char green[3];  // 绿色像素值
};

#endif
