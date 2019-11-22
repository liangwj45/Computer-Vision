#ifndef _Division_
#define _Division_
#include <list>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include "CImg.h"
using std::list;
using std::set;
using std::string;
using std::unordered_map;
using std::vector;
using namespace cimg_library;

typedef unsigned char uchar_t;
#define CIMG CImg<uchar_t>

class Division {
 public:
  Division(string path, string name);
  void divide(int tcon, int tnum, int tdiv, int tsum, const string& img_path,
              const string& num_path);

 private:
  struct Rectangle {
    int lx, ly, rx, ry, area;
    Rectangle(int lx = 0, int ly = 0, int rx = 0, int ry = 0)
        : lx(lx), ly(ly), rx(rx), ry(ry), area((ry - ly) * (rx - lx)) {}
  };

  void Binaryzation(CIMG& img, int tdiv);                   // 二值化
  void Erosion(CIMG in, CIMG& out, int n);                  // 腐蚀
  void Dilation(CIMG in, CIMG& out, int n);                 // 膨胀
  void EliminateConnection(CIMG& img, int tcon, int tnum);  // 消除连通块
  void MarkRectangle(CIMG& img, int lx, int ly, int rx, int ry,
                     uchar_t* color);                    // 标记一个矩形
  void StoreConnection(int lx, int ly, int rx, int ry);  // 记录一个连通块
  void Statistics(int tsum);
  void Statistics(unordered_map<int, int> um, set<int>& result, int tsum);
  void SaveNumber(const string& path);

 private:
  CIMG src;                   // 原图
  CIMG img;                   // 处理图
  CIMG result;                // 结果图
  string name;                // 原图名称
  int width, height;          // 图像宽度和高度
  list<Rectangle> recs;       // 记录连通块位置
  int num_width, num_height;  // 截取数字的保存尺寸
  string path;                // 截取数字的保存路径

 private:
  static int dir[8][2];     // 八个移动方向
  static uchar_t red[3];    // 红色像素值
  static uchar_t blue[3];   // 蓝色像素值
  static uchar_t green[3];  // 绿色像素值
  static int num_cnt;
};

#endif
