#include <cmath>
#include <iostream>
#include "CImg.h"
using namespace cimg_library;

// 定义根号3，等边三角形两条边的方程以及距离公式
#define sqrt3 sqrt(3.0)
#define f1(x) (sqrt3 * x + 20 * sqrt3)
#define f2(x) (-sqrt3 * x + 20 * sqrt3)
#define dis(x, y, x0, y0) (sqrt(pow((x) - (x0), 2) + pow((y) - (y0), 2)))

class HW1 {
public:
	// 1. 展示图像
	static void step1(CImg<unsigned char>& img) {
		img.display();
	}

	// 2. 把图像中白色区域变成红色，黑色区域变成绿色
	static void step2(CImg<unsigned char>& img) {
		cimg_forXY(img, x, y) {
			// 调整参数使边界保持完整
			// if (img(x, y, 0) == 255 && img(x, y, 1) == 255 && img(x, y, 2) == 255) {
			if (img(x, y, 0) >= 120 && img(x, y, 1) >= 120 && img(x, y, 2) >= 120) {
				// 填充为红色
				img(x, y, 0) = 255;
				img(x, y, 1) = 0;
				img(x, y, 2) = 0;
			} else if (img(x, y, 0) == 0 && img(x, y, 1) == 0 && img(x, y, 2) == 0) {
				// 填充为绿色
				img(x, y, 0) = 0;
				img(x, y, 1) = 255;
				img(x, y, 2) = 0;
			}
		}
	}

	// 3. 在图上绘制一个等边三角形区域，其中心坐标(50,50)，边长为 40，填充颜色为蓝色。
	static void step3(CImg<unsigned char>& img) {
		cimg_forXY(img, x, y) {
			// 进行坐标变换
			double xx = (double)x - 50.0;
			double yy = (double)y - 50.0 + 20.0 * sqrt3 / 3.0;
			// 利用线性规划来划分出等边三角形区域
			if (xx >= -20 && xx <= 20 && yy >= 0 && yy <= f1(xx) && yy <= f2(xx)) {
				// 填充为蓝色
				img(x, y, 0) = 0;
				img(x, y, 1) = 0;
				img(x, y, 2) = 255;
			}
		}
	}

	// 4. 在图上绘制一个圆形区域，圆心坐标(50,50)，半径为 15，填充颜色为黄色。
	static void step4(CImg<unsigned char>& img) {
		cimg_forXY(img, x, y) {
			if (dis(x, y, 50, 50) <= 15) {
				img(x, y, 0) = 255;
				img(x, y, 1) = 255;
				img(x, y, 2) = 0;
			}
		}
	}

	// 5. 在图上绘制一条长为 100 的直线段，起点坐标为(0, 0)，方向角为 135 度，直线的颜色为绿色。
	static void step5(CImg<unsigned char>& img) {
		cimg_forXY(img, x, y) {
			if (x == y && dis(x, y, 0, 0) <= 100) {
				img(x, y, 0) = 0;
				img(x, y, 1) = 255;
				img(x, y, 2) = 0;
			}
		}
	}

	// 6. 保存操作结果。
	static void step6(CImg<unsigned char>& img) {
		img.save("./2.bmp");
	}
};