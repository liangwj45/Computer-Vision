#pragma once
#include "CImg.h"
#include <string>
#include <map>
#include <list>
using std::map;
using std::list;
using std::string;
using namespace cimg_library;

class Ex4 {
public:
	Ex4(string path);
	void test();

	struct Point {
		int x, y;
		Point(int x, int y) :x(x), y(y) {}
	};

	struct Rectangle {
		int lx, rx, ly, ry;
	};

private: // Part1
	void RGB2Gray();  // 转灰度图
	void Devision();  // 图像分割

private: // Part2
	void Erosion(CImg<unsigned char> in, CImg<unsigned char>& out, int n);  // 腐蚀
	void Dilation(CImg<unsigned char> in, CImg<unsigned char>& out, int n);  // 膨胀
	void EliminateConnection(int tcon, int tnum);  // 消除连通块
	void FindConnection(int x, int y);  // 寻找连通块
	void DeleteConnection();  // 删除连通块
	void DrawLine(CImg<unsigned char>& img, int bx, int by, int ex, int ey, unsigned char* color, int n); // 画线条
	bool InImage(int x, int y);  // 判断坐标是否在图像中

private: // Part3
	void FindScaleplate(CImg<unsigned char>& img);  // 找刻度尺
	void DrawScaleplate();  // 画刻度尺
	void IdentifyNumber();  // 识别数字
	void SaveNumber();  // 保存原图中的数字作为训练模型
	void LoadModel();  // 加载数字模型
	bool MatchNumber(CImg<unsigned char> src, const CImg<unsigned char>& dest);  // 判断是否匹配数字

private:
	CImg<unsigned char> src;  // 原图
	CImg<unsigned char> img;
	CImg<unsigned char> tmp;
	CImg<unsigned char> nums[10];  // 数字模型
	int width;  // 图像宽度
	int height;  // 图像高度
	int cnt;  // 记录连通块中像素个数
	int lx, rx, ly, ry;  // 记录连通块的边界
	int bx, ex, by, ey;  // 记录刻度尺的边界
	list<Point> con;  // 记录连通块位置
	CImg<bool> vis;  // 记录是否已经遍历
	static int dir[8][2];  // 八个移动方向
	static unsigned char red[3];
	static unsigned char blue[3];
	map<int, Rectangle> recs;
	list<int> result;
	int index = 0;
};
