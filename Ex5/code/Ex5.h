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
  void Devision();  // ͼ��ָ�

 private:  // Part2
  void Erosion(CImg<unsigned char> in, CImg<unsigned char>& out,
               int n);  // ��ʴ
  void Dilation(CImg<unsigned char> in, CImg<unsigned char>& out,
                int n);               // ����
  void EliminateConnection();         // ������ͨ��
  void FindConnection(int x, int y);  // Ѱ����ͨ��
  void DeleteConnection();            // ɾ����ͨ��
  void DrawLine(CImg<unsigned char>& img, int bx, int by, int ex, int ey,
                unsigned char* color, int n);  // ������

 private:                                         // Part3
  void FindScaleplate(CImg<unsigned char>& img);  // �ҿ̶ȳ�
  void DrawScaleplate();                          // ���̶ȳ�

 private:
  CImg<unsigned char> src;  // ԭͼ
  CImg<unsigned char> img;
  CImg<unsigned char> tmp;
  int width;             // ͼ����
  int height;            // ͼ��߶�
  int cnt;               // ��¼��ͨ�������ظ���
  int lx, rx, ly, ry;    // ��¼��ͨ��ı߽�
  int bx, ex, by, ey;    // ��¼�̶ȳߵı߽�
  int con[5000][2];      // ��¼��ͨ��λ��
  CImg<bool> vis;        // ��¼�Ƿ��Ѿ�����
  static int dir[8][2];  // �˸��ƶ�����
  static unsigned char red[3];
  static unsigned char blue[3];
  int tcon;
  int tnum;
};

#endif