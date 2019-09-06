#include <cmath>
#include <iostream>
#include "CImg.h"
using namespace cimg_library;

// �������3���ȱ������������ߵķ����Լ����빫ʽ
#define sqrt3 sqrt(3.0)
#define f1(x) (sqrt3 * x + 20 * sqrt3)
#define f2(x) (-sqrt3 * x + 20 * sqrt3)
#define dis(x, y, x0, y0) (sqrt(pow((x) - (x0), 2) + pow((y) - (y0), 2)))

class HW1 {
public:
	// 1. չʾͼ��
	static void step1(CImg<unsigned char>& img) {
		img.display();
	}

	// 2. ��ͼ���а�ɫ�����ɺ�ɫ����ɫ��������ɫ
	static void step2(CImg<unsigned char>& img) {
		cimg_forXY(img, x, y) {
			// ��������ʹ�߽籣������
			// if (img(x, y, 0) == 255 && img(x, y, 1) == 255 && img(x, y, 2) == 255) {
			if (img(x, y, 0) >= 120 && img(x, y, 1) >= 120 && img(x, y, 2) >= 120) {
				// ���Ϊ��ɫ
				img(x, y, 0) = 255;
				img(x, y, 1) = 0;
				img(x, y, 2) = 0;
			} else if (img(x, y, 0) == 0 && img(x, y, 1) == 0 && img(x, y, 2) == 0) {
				// ���Ϊ��ɫ
				img(x, y, 0) = 0;
				img(x, y, 1) = 255;
				img(x, y, 2) = 0;
			}
		}
	}

	// 3. ��ͼ�ϻ���һ���ȱ���������������������(50,50)���߳�Ϊ 40�������ɫΪ��ɫ��
	static void step3(CImg<unsigned char>& img) {
		cimg_forXY(img, x, y) {
			// ��������任
			double xx = (double)x - 50.0;
			double yy = (double)y - 50.0 + 20.0 * sqrt3 / 3.0;
			// �������Թ滮�����ֳ��ȱ�����������
			if (xx >= -20 && xx <= 20 && yy >= 0 && yy <= f1(xx) && yy <= f2(xx)) {
				// ���Ϊ��ɫ
				img(x, y, 0) = 0;
				img(x, y, 1) = 0;
				img(x, y, 2) = 255;
			}
		}
	}

	// 4. ��ͼ�ϻ���һ��Բ������Բ������(50,50)���뾶Ϊ 15�������ɫΪ��ɫ��
	static void step4(CImg<unsigned char>& img) {
		cimg_forXY(img, x, y) {
			if (dis(x, y, 50, 50) <= 15) {
				img(x, y, 0) = 255;
				img(x, y, 1) = 255;
				img(x, y, 2) = 0;
			}
		}
	}

	// 5. ��ͼ�ϻ���һ����Ϊ 100 ��ֱ�߶Σ��������Ϊ(0, 0)�������Ϊ 135 �ȣ�ֱ�ߵ���ɫΪ��ɫ��
	static void step5(CImg<unsigned char>& img) {
		cimg_forXY(img, x, y) {
			if (x == y && dis(x, y, 0, 0) <= 100) {
				img(x, y, 0) = 0;
				img(x, y, 1) = 255;
				img(x, y, 2) = 0;
			}
		}
	}

	// 6. ������������
	static void step6(CImg<unsigned char>& img) {
		img.save("./2.bmp");
	}
};