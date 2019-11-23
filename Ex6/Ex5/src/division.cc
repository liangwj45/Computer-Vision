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
  bin = Binaryzation(img, tbin);
  bin.save((img_path + name + "_bin" + ".bmp").c_str());
  EliminateConnection(img, tcon, tnum);
  Statistics(tsum);
  Ex5();
  RESAVE("_ex5");
}

void Division::Ex5() {
  cout << "FindNumberArea" << endl;

  // 寻找区域边界
  int number_size = 30, up_number = 3;
  Rectangle up(width, height), down(width, height), mid(width, height);
  for (auto e : recs) {
    if (e.ly < up.ly) up.ly = e.ly;
    if (e.ry > down.ry) down.ry = e.ry;
  }
  for (auto e : recs) {
    if (e.ly - up.ly < up_number * number_size) {
      if (e.lx < up.lx) up.lx = e.lx;
      if (e.rx > up.rx) up.rx = e.rx;
      if (e.ry > up.ry) up.ry = e.ry;
    } else if (down.ry - e.ry < number_size) {
      if (e.lx < down.lx) down.lx = e.lx;
      if (e.ly < down.ly) down.ly = e.ly;
      if (e.rx > down.rx) down.rx = e.rx;
    } else if (down.ry - e.ry < 10 * number_size) {
      if (e.lx < mid.lx) mid.lx = e.lx;
      if (e.ly < mid.ly) mid.ly = e.ly;
      if (e.rx > mid.rx) mid.rx = e.rx;
      if (e.ry > mid.ry) mid.ry = e.ry;
    }
  }
  // MarkRectangle(result, up.lx, up.ly, up.rx, up.ry, red);
  // MarkRectangle(result, mid.lx, mid.ly, mid.rx, mid.ry, red);
  // MarkRectangle(result, down.lx, down.ly, down.rx, down.ry, red);

  // 将数字连通块归类
  vector<Rectangle> up_rec, mid_rec, down_rec;
  for (auto e : recs) {
    if (e.lx >= up.lx && e.ly >= up.ly && e.rx <= up.rx && e.ry <= up.ry) {
      up_rec.push_back(e);
    } else if (e.lx >= mid.lx && e.ly >= mid.ly && e.rx <= mid.rx &&
               e.ry <= mid.ry) {
      mid_rec.push_back(e);
    } else if (e.lx >= down.lx && e.ly >= down.ly && e.rx <= down.rx &&
               e.ry <= down.ry) {
      down_rec.push_back(e);
    }
  }
  std::sort(up_rec.begin(), up_rec.end(),
            [](const Rectangle& a, const Rectangle& b) { return a.lx < b.lx; });
  std::sort(mid_rec.begin(), mid_rec.end(),
            [](const Rectangle& a, const Rectangle& b) { return a.lx < b.lx; });
  std::sort(down_rec.begin(), down_rec.end(),
            [](const Rectangle& a, const Rectangle& b) { return a.lx < b.lx; });

  // 寻找并标记数字（带小数点）
  int up_height = up.ry - up.ly;
  int up_width = up_rec[0].rx - up_rec[0].lx;
  int mid_height = mid.ry - mid.ly;
  int mid_width = mid_rec[0].rx - mid_rec[0].lx;
  int down_height = down.ry - down.ly;
  int down_width;
  if (down_rec[1].lx - down_rec[0].lx > number_size * 2) {
    down_width = down_rec[2].rx - down_rec[1].lx;
  } else {
    down_width = down_rec[1].rx - down_rec[0].lx;
  }
  for (int i = 0; i < up_rec.size(); i++) {
    if (i == 0 || up_rec[i].lx - up_rec[i - 1].lx > up_width) {
      result.draw_rectangle(up_rec[i].lx, up.ly, up_rec[i].lx + up_width, up.ry,
                            red, 0.5);
    }
  }
  for (int i = 0; i < mid_rec.size(); i++) {
    if (i == 0 || mid_rec[i].lx - mid_rec[i - 1].lx > mid_width) {
      result.draw_rectangle(mid_rec[i].lx, mid.ly, mid_rec[i].lx + mid_width,
                            mid.ry, blue, 0.5);
    }
  }
  for (int i = 0; i < down_rec.size() - 1; i++) {
    if (i == 0 || down_rec[i].lx - down_rec[i - 1].lx > down_width) {
      if (down_rec[i + 1].lx - down_rec[i].lx > down_width) {
        result.draw_rectangle(down_rec[i].rx - down_width, down.ly,
                              down_rec[i].rx, down.ry, green, 0.5);
      } else {
        result.draw_rectangle(down_rec[i].lx, down.ly,
                              down_rec[i].lx + down_width, down.ry, green, 0.5);
      }
    }
  }

  cout << "FindBrackets" << endl;
  int up_y, mid_y, down_y, x, y, cnt;
  int max1 = 0, max1_y = 0;
  int max2 = 0, max2_y = 0;
  for (y = 0; y < height; y++) {
    cnt = 0;
    for (x = 0; x < width; x++) {
      cnt += (bin(x, y) == 0);
    }
    if (cnt > max2 && abs(y - max1_y) > 5 && abs(y - max2_y) > 5) {
      max2 = cnt;
      max2_y = y;
      if (max2 > max1) {
        std::swap(max1, max2);
        std::swap(max1_y, max2_y);
      }
    }
  }

  // 寻找括号的纵坐标
  int by = 0, max = 0;
  for (y = max2_y + 5; y < mid.ly - 5; y++) {
    cnt = 0;
    for (x = 0; x < width; x++) {
      cnt += (bin(x, y) == 0);
    }
    if (cnt > max) {
      max = cnt;
      by = y;
    }
  }

  // 寻找左端点
  x = 0;
  while (bin(x, by - 1) || bin(x, by - 2) || bin(x, by - 3)) x++;
  while (!bin(x, by - 1) && !bin(x, by - 2) && !bin(x, by - 3)) x++;
  int lx = --x, mid_x = 0;
  cnt = 0;
  while (x < width) {
    x++;
    // 确定中点
    if (bin(x, by - 1) && bin(x, by - 2) && bin(x, by - 3) && !bin(x, by + 1) &&
        !bin(x, by + 2) && !bin(x, by + 3))
      mid_x = x;
    // 确定右端点
    if (!bin(x, by - 1) && !bin(x, by - 2) && !bin(x, by - 3)) {
      if (!mid_x) {
        // 该右端点实际上是一个左端点
        while (!bin(x, by - 1) && !bin(x, by - 2) && !bin(x, by - 3)) x++;
        lx = --x, mid_x = 0;
      } else {
        // 标记括号
        result.draw_line(lx, by - 3, x, by - 3, green);
        result.draw_line(lx, by - 3, lx, by, green);
        result.draw_line(lx, by, x, by, green);
        result.draw_line(x, by - 3, x, by, green);
        printf("Bracket%d: %d %d %d\n", ++cnt, lx, mid_x, x);
        while (!bin(x, by - 1) && !bin(x, by - 2) && !bin(x, by - 3)) x++;
        // 判断括号是否相连
        if (!bin(x, by) && !bin(x + 1, by) && !bin(x + 2, by)) {
          lx = --x, mid_x = 0;
        } else {
          // 寻找左端点
          while (bin(x, by - 1) || bin(x, by - 2) || bin(x, by - 3)) x++;
          while (!bin(x, by - 1) && !bin(x, by - 2) && !bin(x, by - 3)) x++;
          lx = --x, mid_x = 0;
        }
      }
    }
  }
}

void Division::DrawRectangle(CIMG& img, const Rectangle& rec, const char* color,
                             int opacity) {
  img.draw_rectangle(rec.lx, rec.ly, rec.rx, rec.ry, color, opacity);
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
CIMG Division::Binaryzation(CIMG& img, int tbin) {
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
  return img;
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
    heap.pop();
  }
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
  // for (auto e : recs) {
  //  MarkRectangle(result, e.lx, e.ly, e.rx, e.ry, red);
  //}
}
