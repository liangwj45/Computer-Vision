#include "Ex4.h"
#include <algorithm>
#include <stdio.h>
#include <string>
#include <iostream>
using std::endl;
using std::cout;
using std::list;

int Ex4::dir[8][2] = { -1,0,1,0,0,-1,0,1,-1,-1,-1,1,1,-1,1,1 };
unsigned char Ex4::red[3] = { 255, 0, 0 };
unsigned char Ex4::blue[3] = { 0, 0, 255 };

Ex4::Ex4(string path) {
	index = 0;
	img = CImg<unsigned char>(path.c_str());
	src = img;
	width = img._width, height = img._height;
	vis.resize(width, height);
}

void Ex4::test() {
	LoadModel();
	Erosion(img, img, 3);
	img.save("./img/2-erosion.bmp");
	FindScaleplate(img);
	EliminateConnection(300, 80);
	img.save("./img/3-eliminate_connection.bmp");
	src.save("./img/4-draw_frame.bmp");
	DrawScaleplate();
	src.save("./img/5-draw_scaleplate.bmp");
	IdentifyNumber();
}

void Ex4::Erosion(CImg<unsigned char> in, CImg<unsigned char>& out, int n) {
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
		out(i, j, 0) = out(i, j, 1) = out(i, j, 2) = flag ? 0 : 255;
	}
}

void Ex4::Dilation(CImg<unsigned char> in, CImg<unsigned char>& out, int n) {
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
		out(i, j, 0) = out(i, j, 1) = out(i, j, 2) = flag ? 255 : 0;
	}
}

void Ex4::EliminateConnection(int tcon, int tnum) {
	cout << "EliminateConnection" << endl;
	cimg_forXY(vis, i, j) {
		vis(i, j) = 0;
	}
	cimg_forXY(img, i, j) {
		if (!img(i, j) && !vis(i, j)) {
			cnt = 1, lx = rx = i, ly = ry = j;
			con.clear();
			FindConnection(i, j);
			if (cnt > tcon || cnt < tnum) {
				DeleteConnection();
			}
			else {
				int side = 2;
				lx -= side, ly -= side, rx += side, ry += side;
				src.draw_line(lx, ly, rx, ly, red);
				src.draw_line(lx, ly, lx, ry, red);
				src.draw_line(rx, ly, rx, ry, red);
				src.draw_line(lx, ry, rx, ry, red);


				if (lx > bx && rx < ex && ly > by && ry < ey) {
					recs.emplace(lx, Rectangle({ lx, rx, ly, ry }));
				}
			}
		}
	}
}

void Ex4::FindConnection(int x, int y) {
	con.emplace_back(x, y);
	lx = std::min(lx, x);
	ly = std::min(ly, y);
	rx = std::max(rx, x);
	ry = std::max(ry, y);
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

void Ex4::FindScaleplate(CImg<unsigned char>& img) {
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

void Ex4::DrawScaleplate() {
	cout << "DrawScaleplate" << endl;
	DrawLine(src, bx, by, ex, ey, blue, 5);
	DrawLine(src, bx, ey, ex, ey, blue, 5);
	DrawLine(src, bx, by, bx, ey, blue, 5);
	DrawLine(src, ex, by, ex, ey, blue, 5);
}

void Ex4::IdentifyNumber() {
	cout << "IdentifyNumber" << endl;
	result.clear();
	for (auto each : recs) {
		Rectangle rec = each.second;
		CImg<unsigned char> tmp;
		int width = rec.rx - rec.lx + 1;
		int height = rec.ry - rec.ly + 1;
		tmp.resize(width, height, 1);
		cimg_forXY(tmp, x, y) {
			tmp(x, y) = src(x + rec.lx, y + rec.ly);
		}
		for (int i = 0; i <= 9; i++) {
			if (MatchNumber(tmp, nums[i])) {
				result.push_back(i);
				break;
			}
		}
	}
	for (int num : result) {
		cout << num << " ";
	}
	cout << endl;
}

bool Ex4::MatchNumber(CImg<unsigned char> src, const CImg<unsigned char>& dest) {
	int area = dest._width * dest._height, dif = 0;
	src.resize(dest._width, dest._height, 1);
	cimg_forXY(src, i, j) {
		if (src(i, j) != dest(i, j)) {
			dif++;
		}
	}
	return (double)dif / area < 0.14;
}

void Ex4::RGB2Gray() {
	cout << "RGB2Gray" << endl;
	int r = 0, g = 0, b = 0;
	cimg_forXY(img, x, y) {
		r = img(x, y, 0);
		g = img(x, y, 1);
		b = img(x, y, 2);
		img(x, y, 0) = img(x, y, 1) = img(x, y, 2) = (r * 0.2 + g * 0.7 + b * 0.1);
	}
	img.resize(img._width, img._height, 1, 1);
}

void Ex4::Devision() {
	cout << "Devision" << endl;
	int cnt[256];
	for (int i = 0; i < 256; i++) {
		cnt[i] = 0;
	}
	cimg_forXY(img, x, y) {
		cnt[img(x, y, 0)]++;
	}
	int min = cnt[0];
	for (int i = 1; i < 256; i++) {
		min = std::min(cnt[i], min);
	}
	cimg_forXY(img, x, y) {
		if (img(x, y, 0) < min) {
			img(x, y, 0) = img(x, y, 1) = img(x, y, 2) = 0;
		}
		else {
			img(x, y, 0) = img(x, y, 1) = img(x, y, 2) = 255;
		}
	}
}

void Ex4::DrawLine(CImg<unsigned char>& img, int bx, int by, int ex, int ey, unsigned char* color, int n) {
	if (bx == ex) {
		for (int i = -n / 2; i < n / 2; i++) {
			img.draw_line(bx + i, by, bx + i, ey, color);
		}
	}
	else {
		for (int i = -n / 2; i < n / 2; i++) {
			img.draw_line(bx, by + i, ex, by + i, color);
		}
	}
}

bool Ex4::InImage(int x, int y) {
	return x >= 0 && x < width && y >= 0 && y < height;
}

void Ex4::SaveNumber() {
	cout << "SaveNumber" << endl;
	index++;
	tmp.resize(rx - lx + 1, ry - ly + 1);
	cimg_forXY(tmp, p, q) {
		tmp(p, q) = src(p + lx, q + ly, 0);
	}
	tmp.save(("training/" + std::to_string(index) + ".bmp").c_str());
}

void Ex4::LoadModel() {
	cout << "LoadModel" << endl;
	for (int i = 0; i <= 9; i++) {
		nums[i] = CImg<unsigned char>(("num/" + std::to_string(i) + ".bmp").c_str());
		nums[i].resize(16, 25, 1);
	}
}
