#include "Ex5.h"
#include <stdio.h>
#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <vector>
using std::cout;
using std::endl;
using std::map;
using std::set;
using std::vector;

int Ex5::dir[8][2] = {-1, 0, 1, 0, 0, -1, 0, 1, -1, -1, -1, 1, 1, -1, 1, 1};
unsigned char Ex5::red[3] = {255, 0, 0};
unsigned char Ex5::blue[3] = {0, 0, 255};
unsigned char Ex5::green[3] = {0, 255, 0};

#define InImage(x, y) ((x) >= 0 && (x) < width && (y) >= 0 && (y) < height)
#define vis(x, y) vis[(x)*height + (y)]
#define con(x, y) con[(x)*2 + (y)]

Ex5::Ex5(string path, string name) : name(name) {
  result = src = img = CImg<unsigned char>(path.c_str());
  img.resize(img._width, img._height, 1, 1);
  width = img._width, height = img._height;
  vis = new bool[width * height];
  con = new int[width * height * 2];
  rec.resize(width * height);
  for (int i = 0; i < width * height; i++) {
    vis[i] = 0;
  }
}

Ex5::~Ex5() {
  delete vis;
  delete con;
  vis = nullptr;
  con = nullptr;
}

void Ex5::test(int tcon, int tnum, int tdiv) {
  Devision(tdiv);
  div = img;
  // img.save(("./img/" + name + "_2-devision.bmp").c_str());
  Erosion(img, img, 3);
  // img.save(("./img/" + name + "_3-erosion.bmp").c_str());
  FindScaleplate(img);
  EliminateConnection(tcon, tnum);
  // img.save(("./img/" + name + "_4-eliminate_connection.bmp").c_str());
  // result.save(("./img/" + name + "_5-draw_frame.bmp").c_str());
  // DrawScaleplate();
  // result.save(("./img/" + name + "_6-draw_scaleplate.bmp").c_str());
  Summary();
  FindNumberAreas();
  // result.save(("./img/" + name + "_8-draw_rectangle.bmp").c_str());
  IdentifyNumber();
  // result.save(("./img/" + name + "_9-draw_rectangle.bmp").c_str());
  ComputeCoordinate();
  FindBracket();
  result.save(("./img/" + name + "_10-draw_brackets.bmp").c_str());
}

void Ex5::FindBracket() {
  cout << "FindBracket" << endl;
  // 找括号的纵坐标
  int max = 0;
  for (int i = down.ly - 20; i < down.ly; i++) {
    int cnt = 0, lx = width, rx = 0;
    for (int j = 0; j < width; j++) {
      if (src(j, i) < 200) lx = std::min(lx, j), rx = std::max(rx, j), cnt++;
    }
    if (cnt > max) max = cnt, cy = i, clx = lx, crx = rx;
  }

  // 设置默认左端点
  int i = clx, lx = i, cnt;
  if (name == "2") lx++, i++;
  while (i < crx) {
    // 确定右端点
    i++, cnt = 0;
    for (int j = 0; j < 4; j++) {
      if (src(i, cy - j) < 150) cnt++;
    }
    if (cnt >= 3) {
      // 保存括号位置
      brackets.emplace_back(lx, i, cy, (lx + i) / 2);
      if (brackets.size() == 4 && name == "1") cy += 1;
      cnt = 0;
      // 判断两个括号是否相连
      for (int j = 0; j < 4; j++) {
        if (src(i + j, cy) < 150) cnt++;
      }
      if (cnt >= 4) {
        lx = i;
      } else {
        i += name == "1" ? 1 : 2;
        while (true) {
          cnt = 0;
          for (int j = 0; j < 4; j++) {
            if (src(i, cy - j) < 150) cnt++;
          }
          if (cnt >= 3) {
            if (name == "2" && brackets.size() != 2 && brackets.size() != 3)
              i++;
            lx = i;
            break;
          }
          i++;
        }
      }
    }
  }
  cout << brackets.size() << endl;
  for (auto each : brackets) {
    DrawRectangle(each.lx, each.y - 6, each.rx, each.y, red);
    float value = 0;
    for (int i = 0; i < bracket_nums.size(); i++) {
      if (each.mid > bracket_nums[i].lx && each.mid < bracket_nums[i].rx) {
        CImg<unsigned char> tmp;
        SubImage(bracket_nums[i], tmp);
        value = identify.Predict(tmp);
        break;
      }
    }
    cout.setf(std::ios::fixed);
    cout << value << " " << Calculate(each.lx) << " " << Calculate(each.rx)
         << endl;
  }
}

void Ex5::ComputeCoordinate() {
  int cnt, x1 = bx, x2 = ex;
  while (true) {
    cnt = 0;
    for (int i = 0; i < 4; i++) {
      if (src(x2, by + 2 + i) < 150) cnt++;
    }
    if (cnt >= 3) {
      break;
    }
    x2--;
  }
  while (true) {
    cnt = 0;
    for (int i = 0; i < 4; i++) {
      if (src(x1, by + 2 + i) < 150) cnt++;
    }
    if (cnt >= 3) {
      break;
    }
    x1++;
  }
  cbx = x1, cex = x2;
}

float Ex5::Calculate(int x) {
  return name == "1" ? 5.2 * (cex - x) / (cex - cbx) + 0.6
                     : 8.0 * (cex - x) / (cex - cbx) - 0.1;
}

void Ex5::Summary() {
  cout << "Summary" << endl;
  map<int, int> area;
  for (int i = 0; i < cnt_rec; i++) {
    if (area.find(rec[i].area) == area.end()) {
      area.emplace(rec[i].area, 1);
    } else {
      area[rec[i].area]++;
    }
  }
  for (auto e : area) {
    cnt_area.push_back(e);
  }
  std::sort(cnt_area.begin(), cnt_area.end(),
            [](std::pair<int, int> a, std::pair<int, int> b) {
              return a.second > b.second;
            });
  for (auto each : cnt_area) {
    if (each.second > 5) {
      set_area.insert(each.first);
    } else {
      break;
    }
  }
}

void Ex5::FindNumberAreas() {
  cout << "FindNumberAreas" << endl;
  up.lx = down.lx = width, up.ly = down.ly = height;
  up.rx = down.rx = 0, up.ry = down.ry = 0;
  for (int i = 0; i < rec.size(); i++) {
    if (set_area.find(rec[i].area) != set_area.end() ||
        (rec[i].lx >= bx && rec[i].rx <= ex && rec[i].ly >= by &&
         rec[i].ry <= ey)) {
      if (rec[i].ly < 50 || rec[i].ry > height - 20) continue;
      if (rec[i].ly < height / 3) {
        up.lx = std::min(up.lx, rec[i].lx);
        up.ly = std::min(up.ly, rec[i].ly);
        up.rx = std::max(up.rx, rec[i].rx);
        up.ry = std::max(up.ry, rec[i].ry);
        if (up_rec.find(rec[i].lx) == up_rec.end())
          up_rec.emplace(rec[i].lx, vector<Rectangle>());
        up_rec[rec[i].lx].push_back(rec[i]);
      } else if (rec[i].ly > ey || rec[i].ry < by) {
        down.lx = std::min(down.lx, rec[i].lx);
        down.ly = std::min(down.ly, rec[i].ly);
        down.rx = std::max(down.rx, rec[i].rx);
        down.ry = std::max(down.ry, rec[i].ry);
        if (down_rec.find(rec[i].lx) == down_rec.end())
          down_rec.emplace(rec[i].lx, vector<Rectangle>());
        down_rec[rec[i].lx].push_back(rec[i]);
      } else {
        if (ruler_rec.find(rec[i].lx) == ruler_rec.end())
          ruler_rec.emplace(rec[i].lx, vector<Rectangle>());
        ruler_rec[rec[i].lx].push_back(rec[i]);
      }
    }
  }
}

void Ex5::IdentifyNumber() {
  cout << "IdentifyNumber" << endl;
  // up
  list<int> delist;
  vector<Rectangle>& prev = (*up_rec.begin()).second;
  int w = prev[0].width;
  for (auto each : up_rec) {
    if (each.first == prev[0].lx) continue;
    if (each.second[0].lx - prev[0].lx < w / 4) {
      for (auto r : each.second) prev.push_back(r);
      delist.push_back(each.first);
    }
    prev = each.second;
  }
  for (auto each : delist) up_rec.erase(each);
  for (auto each : up_rec) {
    auto& vr = each.second;
    std::sort(vr.begin(), vr.end(),
              [](Rectangle a, Rectangle b) { return a.ly > b.ly; });
    result.draw_rectangle(each.first, up.ly, each.first + w, up.ry, red, 0.5);
  }
  // down
  prev = (*down_rec.begin()).second;
  w = prev[0].width;
  int tw = name == "1" ? w * 4 : w / 4;
  for (auto each : down_rec) {
    if (each.first == prev[0].lx) continue;
    if (each.second[0].lx - prev[0].lx < tw) {
      for (auto r : each.second) prev.push_back(r);
      delist.push_back(each.first);
    } else {
      prev = each.second;
    }
  }
  for (auto each : delist) down_rec.erase(each);
  if (name == "1") w = prev[0].width * 4;
  for (auto each : down_rec) {
    auto& vr = each.second;
    std::sort(vr.begin(), vr.end(),
              [](Rectangle a, Rectangle b) { return a.ly > b.ly; });
    if (name == "2") {
      result.draw_rectangle(each.first, down.ly, each.first + w, down.ry, green,
                            0.5);
      bracket_nums.emplace_back(each.first, down.ly, each.first + w, down.ry);
    } else {
      result.draw_rectangle(each.first, vr[0].ly, each.first + w, vr[0].ry,
                            green, 0.5);
      bracket_nums.emplace_back(each.first, vr[0].ly, each.first + w, vr[0].ry);
    }
  }
  // ruler
  prev = (*ruler_rec.begin()).second;
  tw = prev[0].width * 3;
  for (auto each : ruler_rec) {
    if (each.first == prev[0].lx) continue;
    if (each.second[0].lx - prev[0].lx < tw) {
      for (auto r : each.second) prev.push_back(r);
      delist.push_back(each.first);
    } else {
      prev = each.second;
    }
  }
  for (auto each : delist) ruler_rec.erase(each);
  w = prev[1].rx - prev[0].lx;
  for (auto each : ruler_rec) {
    auto& vr = each.second;
    std::sort(vr.begin(), vr.end(),
              [](Rectangle a, Rectangle b) { return a.ly > b.ly; });
    result.draw_rectangle(each.first, vr[0].ly, each.first + w, vr[0].ry, blue,
                          0.5);
  }
}

void Ex5::DrawRectangle(int lx, int ly, int rx, int ry, unsigned char* color) {
  result.draw_line(lx, ly, rx, ly, color);
  result.draw_line(lx, ly, lx, ry, color);
  result.draw_line(rx, ly, rx, ry, color);
  result.draw_line(lx, ry, rx, ry, color);
}

void Ex5::SubImage(const Rectangle& rec, CImg<unsigned char>& out) {
  out.resize(rec.width, rec.height, 1, 1);
  cimg_forXY(out, x, y) { out(x, y) = src(x + rec.lx, y + rec.ly); }
}

// 消除过大或过小连通块并记录有效连通块
void Ex5::EliminateConnection(int tcon, int tnum) {
  cout << "EliminateConnection" << endl;
  cimg_forXY(img, i, j) {
    if (!img(i, j) && !vis(i, j)) {
      FindConnection(i, j);
      if (cnt_con > tcon || cnt_con < tnum) {
        DeleteConnection();
      } else {
        // lx -= 2, ly -= 2, rx += 2, ry += 2;
        // result.draw_line(lx, ly, rx, ly, red);
        // result.draw_line(lx, ly, lx, ry, red);
        // result.draw_line(rx, ly, rx, ry, red);
        // result.draw_line(lx, ry, rx, ry, red);
        rec[cnt_rec].lx = lx;
        rec[cnt_rec].ly = ly;
        rec[cnt_rec].rx = rx;
        rec[cnt_rec].ry = ry;
        rec[cnt_rec].width = rx - lx + 1;
        rec[cnt_rec].height = ry - ly + 1;
        rec[cnt_rec].area = rec[cnt_rec].width * rec[cnt_rec].height;
        cnt_rec++;
      }
    }
  }
}

// 寻找连通块
void Ex5::FindConnection(int x, int y) {
  cnt_con = 0, lx = rx = x, ly = ry = y;
  vis(x, y) = 1, con(cnt_con, 0) = x, con(cnt_con, 1) = y;
  int tail = 0;
  while (tail <= cnt_con) {
    for (int i = 0; i < 8; i++) {
      int xx = dir[i][0] + con(tail, 0);
      int yy = dir[i][1] + con(tail, 1);
      if (xx < lx) lx = xx;
      if (yy < ly) ly = yy;
      if (xx > rx) rx = xx;
      if (yy > ry) ry = yy;
      if (InImage(xx, yy) && !vis(xx, yy) && !img(xx, yy)) {
        ++cnt_con, vis(xx, yy) = 1, con(cnt_con, 0) = xx, con(cnt_con, 1) = yy;
      }
    }
    tail++;
  }
}

// 删除特定连通块
void Ex5::DeleteConnection() {
  for (int i = 0; i < cnt_con; i++) {
    img(con(i, 0), con(i, 1)) = 255;
  }
}

// 寻找刻度尺
void Ex5::FindScaleplate(CImg<unsigned char>& img) {
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
  int suby = 2, addy = 43, dx = 2;
  ey = by + addy, bx -= dx, ex += dx, by -= suby;
}

// 标记刻度尺
void Ex5::DrawScaleplate() {
  cout << "DrawScaleplate" << endl;
  DrawLine(result, bx, by, ex, ey, blue, 5);
  DrawLine(result, bx, ey, ex, ey, blue, 5);
  DrawLine(result, bx, by, bx, ey, blue, 5);
  DrawLine(result, ex, by, ex, ey, blue, 5);
}

// 标记矩形区域
void Ex5::DrawRectangle(const Rectangle& rec, unsigned char* color) {
  result.draw_line(rec.lx, rec.ly, rec.rx, rec.ly, color);
  result.draw_line(rec.lx, rec.ly, rec.lx, rec.ry, color);
  result.draw_line(rec.rx, rec.ly, rec.rx, rec.ry, color);
  result.draw_line(rec.lx, rec.ry, rec.rx, rec.ry, color);
}

// 标记横线
void Ex5::DrawLine(CImg<unsigned char>& img, int bx, int by, int ex, int ey,
                   unsigned char* color, int n) {
  if (bx == ex) {
    for (int i = -n / 2; i < n / 2; i++) {
      img.draw_line(bx + i, by, bx + i, ey, color);
    }
  } else {
    for (int i = -n / 2; i < n / 2; i++) {
      img.draw_line(bx, by + i, ex, by + i, color);
    }
  }
}

// 前后景分割
void Ex5::Devision(int tdiv) {
  cout << "Devision" << endl;
  if (tdiv == 0) {
    // 统计频率
    int cnt_con[256];
    for (int i = 0; i < 256; i++) cnt_con[i] = 0;
    cimg_forXY(img, x, y) { cnt_con[img(x, y)]++; }
    // 找双峰
    int maxi1 = 0, maxi2 = 0;
    for (int i = 1; i < 256; i++) {
      if (cnt_con[i] > cnt_con[maxi1]) {
        maxi2 = maxi1, maxi1 = i;
      } else if (cnt_con[i] > cnt_con[maxi2]) {
        maxi2 = i;
      }
    }
    // 找谷底
    if (maxi1 > maxi2) std::swap(maxi1, maxi2);
    int mini = maxi1;
    for (int i = maxi1 + 1; i < maxi2; i++) {
      if (cnt_con[i] < cnt_con[mini]) {
        mini = i;
      }
    }
    tdiv = mini;
  }
  // 二值化
  cimg_forXY(img, x, y) { img(x, y) = img(x, y) < tdiv ? 0 : 255; }
}

// 腐蚀
void Ex5::Erosion(CImg<unsigned char> in, CImg<unsigned char>& out, int n) {
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
    out(i, j) = flag ? 0 : 255;
  }
}

// 膨胀
void Ex5::Dilation(CImg<unsigned char> in, CImg<unsigned char>& out, int n) {
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
    out(i, j) = flag ? 255 : 0;
  }
}

Ex5::Rectangle::Rectangle(int lx, int ly, int rx, int ry, int width, int height,
                          int area)
    : lx(lx),
      ly(ly),
      rx(rx),
      ry(ry),
      width(width),
      height(height),
      area(area) {}

Ex5::Bracket::Bracket(int lx, int rx, int y, int mid)
    : lx(lx), rx(rx), y(y), mid(mid) {}
