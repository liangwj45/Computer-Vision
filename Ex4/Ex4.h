#pragma once
#include "CImg.h"
#include <string>
#include <list>
using std::list;
using std::string;
using namespace cimg_library;

class Ex4 {
public:
	Ex4(string path);
	void test();


	struct Point {
		int x, y;
	};

private:
	void Devision();
	void Erosion();
	void Erosion(CImg<unsigned char> in, CImg<unsigned char>& out, int n);
	void Dilation(CImg<unsigned char> in, CImg<unsigned char>& out, int n);
	void EliminateConnection(int tcon, int tnum);

private:
	void RGB2Gray();
	void PrintPixelDistribution(int cnt[256]);
	void FindConnection(int x, int y);
	void DeleteConnection();
	bool InImage(int x, int y);

private:
	CImg<unsigned char> img;
	CImg<unsigned char> src;
	int width;  // 图像宽度
	int height;   // 图像高度
	int cnt;  // 记录连通块中像素个数
	int lx, rx, ly, ry;  // 记录连通块的边界
	list<Point> con;  // 记录连通块位置
	CImg<bool> vis;  // 记录是否已经遍历
	static int dir[8][2];  // 八个移动方向
	static unsigned char red[3];
	static unsigned char blue[3];
};
