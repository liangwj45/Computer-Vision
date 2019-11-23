#include "Division.h"
#include <stdio.h>
#include <algorithm>
#include <iostream>
#include <queue>
using std::cout;
using std::endl;
using std::priority_queue;

#define RESAVE(tag) result.save((img_path + name + tag + ".bmp").c_str())
#define IMSAVE(tag) img.save((img_path + name + tag + ".bmp").c_str())

int Division::num_cnt = 0;
int Division::dir[8][2] = {-1, 0,  1,  0, 0, -1, 0, 1,
                           -1, -1, -1, 1, 1, -1, 1, 1};
uchar_t Division::red[3] = {255, 0, 0};
uchar_t Division::blue[3] = {0, 0, 255};
uchar_t Division::green[3] = {0, 255, 0};

#define InImage(x, y) ((x) >= 0 && (x) < width && (y) >= 0 && (y) < height)
#define vis(x, y) vis[(x)*height + (y)]
#define con(x, y) con[(x)*2 + (y)]

Division::Division(string path, string name) : name(name) {
  result = src = img = CIMG(path.c_str());
  img.resize(img._width, img._height, 1, 1);
  width = img._width, height = img._height;

  num_width = 16, num_height = 16;
}

void Division::divide(int tcon, int tnum, int tbin, int tsum,
                      const string& img_path, const string& num_path) {
  Binaryzation(img, tbin);
  EliminateConnection(img, tcon, tnum);
  Statistics(tsum);
  RESAVE("_5-sum");
  return;
}

// 消除过大或过小连通块并记录有效连通块
void Division::EliminateConnection(CIMG& img, int tcon, int tnum) {
  cout << "EliminateConnection" << endl;
  int* con = new int[width * height * 2];  // 记录连通块像素位置
  bool* vis = new bool[width * height];    // 记录已遍历的像素位置
  memset(vis, 0, sizeof(bool) * width * height);

  int cnt_con, lx, rx, ly, ry, tail, xx, yy;
  // 遍历图像
  cimg_forXY(img, x, y) {
    // 未遍历且像素值为 0
    if (!img(x, y) && !vis(x, y)) {
      cnt_con = 0;  // 记录当前连通块像素个数
      vis(x, y) = 1;
      con(cnt_con, 0) = x;
      con(cnt_con, 1) = y;
      lx = rx = x;
      ly = ry = y;
      tail = 0;
      // bfs查找
      while (tail <= cnt_con) {
        for (int i = 0; i < 8; i++) {
          xx = dir[i][0] + con(tail, 0);
          yy = dir[i][1] + con(tail, 1);
          if (xx < lx) lx = xx;
          if (yy < ly) ly = yy;
          if (xx > rx) rx = xx;
          if (yy > ry) ry = yy;
          if (InImage(xx, yy) && !vis(xx, yy) && !img(xx, yy)) {
            ++cnt_con;
            vis(xx, yy) = 1;
            con(cnt_con, 0) = xx;
            con(cnt_con, 1) = yy;
          }
        }
        tail++;
      }
      // 大于某阈值或小于某阈值则删除
      if (cnt_con > tcon || cnt_con < tnum) {
        for (int i = 0; i < cnt_con; i++) {
          img(con(i, 0), con(i, 1)) = 255;
        }
      } else {
        // MarkRectangle(result, lx, ly, rx, ry, red);
        StoreConnection(lx, ly, rx, ry);
      }
    }
  }

  delete[] vis;
  delete[] con;
}

// 二值化处理
void Division::Binaryzation(CIMG& img, int tbin) {
  cout << "Binaryzation" << endl;
  if (tbin == 0) {
    int cnt[256], m1 = 0, m2 = 0, min, i, j;
    // 统计像素直方图
    for (i = 0; i < 256; i++) cnt[i] = 0;
    cimg_forXY(img, x, y) { cnt[img(x, y)]++; }
    // 直方图平滑
    for (i = 0; i < 1000; i++)
      for (j = 1; j < 255; j++) {
        cnt[j] = (cnt[j - 1] + cnt[j] + cnt[j + 1]) / 3;
      }
    // 寻找双峰
    for (i = 1; i < 256; i++) {
      if (cnt[i] > cnt[m1]) {
        m2 = m1, m1 = i;
      } else if (cnt[i] > cnt[m2]) {
        m2 = i;
      }
    }
    // 寻找谷底
    if (m1 > m2) std::swap(m1, m2);
    min = m1;
    for (i = m1 + 1; i < m2; i++) {
      if (cnt[i] < cnt[min]) {
        min = i;
      }
    }
    tbin = min;
  }
  // 二值化
  cimg_forXY(img, x, y) { img(x, y) = img(x, y) < tbin ? 0 : 255; }
}

// 腐蚀
void Division::Eroding(CIMG in, CIMG& out, int n) {
  cout << "Eroding" << endl;
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
    out(i, j) = flag ? 0 : 255;
  }
}

// 膨胀
void Division::Dilating(CIMG in, CIMG& out, int n) {
  cout << "Dilating" << endl;
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
    out(i, j) = flag ? 255 : 0;
  }
}

// 标记矩形区域
void Division::MarkRectangle(CIMG& img, int lx, int ly, int rx, int ry,
                             uchar_t* color) {
  img.draw_line(lx, ly, rx, ly, color);
  img.draw_line(lx, ly, lx, ry, color);
  img.draw_line(rx, ly, rx, ry, color);
  img.draw_line(lx, ry, rx, ry, color);
}

// 保存连通块
void Division::StoreConnection(int lx, int ly, int rx, int ry) {
  recs.push_back(Rectangle({lx, ly, rx, ry}));
}

typedef std::pair<int, int> record_t;
struct RecordComparer {
  bool operator()(const record_t& a, const record_t& b) {
    return a.second > b.second;
  }
};

// 使用最小堆解决 Top K 问题
void Division::Statistics(unordered_map<int, int> um, set<int>& result,
                          int tsum) {
  priority_queue<record_t, vector<record_t>, RecordComparer> heap;
  auto it = um.begin();
  for (int i = 0; i < tsum && it != um.end(); i++, it++) heap.push(*it);
  for (; it != um.end(); it++) {
    if (it->second > heap.top().second) {
      heap.pop(), heap.push(*it);
    }
  }
  while (!heap.empty()) {
    result.insert(heap.top().first);
    // printf("%d ", heap.top().first);
    heap.pop();
  }
  // puts("");
}

// 统计位置信息
void Division::Statistics(int tsum) {
  cout << "Statistics" << endl;
  set<int> lxs, lys, rxs, rys, areas;
  unordered_map<int, int> lxm, lym, rxm, rym, aream;
  for (auto e : recs)
    lxm[e.lx]++, lym[e.ly]++, rxm[e.rx]++, rym[e.ry]++, aream[e.area]++;
  Statistics(lxm, lxs, tsum);
  Statistics(lym, lys, tsum);
  Statistics(rxm, rxs, tsum);
  Statistics(rym, rys, tsum);
  Statistics(aream, areas, tsum);
  bool lx, ly, rx, ry, area;
  for (auto it = recs.begin(); it != recs.end();) {
    lx = (lxs.find(it->lx) != lxs.end());
    ly = (lys.find(it->ly) != lys.end());
    rx = (rxs.find(it->rx) != rxs.end());
    ry = (rys.find(it->ry) != rys.end());
    area = (areas.find(it->area) != areas.end());
    // 面积不满足符合大多数特征，或四个边界特征没有一个满足则移除
    if (!area || lx + ly + rx + ry < 1) {
      it = recs.erase(it);
    } else {
      it++;
    }
  }
  printf("amount of number: %d\n", recs.size());
  for (auto e : recs) {
    MarkRectangle(result, e.lx, e.ly, e.rx, e.ry, red);
  }
}
