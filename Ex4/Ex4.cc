﻿#include "Ex4.h"
#include <list>
#include <queue>
using std::list;
using std::queue;
#include <iostream>
using std::cout;
using std::endl;

int Ex4::dir[8][2] = { -1,0,1,0,0,-1,0,1,-1,-1,-1,1,1,-1,1,1 };

Ex4::Ex4(string path) {
	img = CImg<unsigned char>(path.c_str());
	width = img._width, height = img._height;
	vis.resize(width, height);
}

void Ex4::test() {
	Erosion(img, img, 3);
	img.save("./img/2.bmp");
	EliminateConnection(300);
	img.save("./img/5.bmp");
	return;
}

void Ex4::EliminateConnection(int tcon) {
	cout << "EliminateConnection" << endl;
	cimg_forXY(vis, i, j) {
		vis(i, j) = 0;
	}
	cimg_forXY(img, i, j) {
		if (!img(i, j) && !vis(i, j)) {
			cnt = 1;
			con.clear();
			FindConnection(i, j);
			if (cnt > tcon) {
				DeleteConnection();
			}
		}
	}
}

void Ex4::FindConnection(int x, int y) {
	con.push_back(Point({ x, y }));
	vis(x, y) = 1;
	for (int i = 0; i < 8; i++) {
		int xx = dir[i][0] + x;
		int yy = dir[i][1] + y;
		if (InImage(xx, yy) && !vis(xx, yy) && !img(xx, yy)) {
			++cnt;
			FindConnection(xx, yy);
		}
	}
}

void Ex4::DeleteConnection() {
	for (Point p : con) {
		img(p.x, p.y, 0) = img(p.x, p.y, 1) = img(p.x, p.y, 2) = 255;
	}
}


void Ex4::Erosion(CImg<unsigned char> in, CImg<unsigned char>& out, int n) {
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
		out(i, j, 0) = out(i, j, 1) = out(i, j, 2) = flag ? 0 : 255;
	}
}

void Ex4::Dilation(CImg<unsigned char> in, CImg<unsigned char>& out, int n) {
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
		out(i, j, 0) = out(i, j, 1) = out(i, j, 2) = flag ? 255 : 0;
	}
}

bool Ex4::InImage(int x, int y) {
	return x >= 0 && x < width && y >= 0 && y < height;
}


void Ex4::Devision() {
	int cnt[256];
	for (int i = 0; i < 256; i++) {
		cnt[i] = 0;
	}
	cimg_forXY(img, x, y) {
		cnt[img(x, y, 0)]++;
	}
	PrintPixelDistribution(cnt);
}

void Ex4::RGB2Gray() {
	int r = 0, g = 0, b = 0;
	cimg_forXY(img, x, y) {
		r = img(x, y, 0);
		g = img(x, y, 1);
		b = img(x, y, 2);
		img(x, y, 0) = img(x, y, 1) = img(x, y, 2) = (r * 0.2 + g * 0.7 + b * 0.1);
	}
	img.resize(img._width, img._height, 1, 1);
}

void Ex4::PrintPixelDistribution(int cnt[256]) {
	for (int i = 0; i < 256; i++) {
		if (cnt[i] > 0) {
			printf("%d %d\n", i, cnt[i]);
		}
	}
}