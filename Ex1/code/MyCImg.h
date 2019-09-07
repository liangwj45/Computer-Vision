#pragma once
#include "CImg.h"
using namespace cimg_library;

// 定义根号3以及等边三角形两条边的方程
#define sqrt3 sqrt(3.0)
#define f1(x) (sqrt3 * x + 20 * sqrt3)
#define f2(x) (-sqrt3 * x + 20 * sqrt3)

class MyCImg {
 public:
  MyCImg(CImg<unsigned char>& img) : img(img) {}

  /**
   * @brief 展示一张图像
   *
   * @param title 图像标题
   * @return const MyCImg& 返回当前对象
   */
  const MyCImg& display(const char* const title = 0) const {
    img.display();
    return *this;
  }

  /**
   * @brief 更换图像颜色
   *
   * @param from 需要更换的颜色
   * @param to 更换成的颜色
   * @return MyCImg& 返回当前对象
   */
  MyCImg& changeColor(const unsigned char* const from,
                      const unsigned char* const to) {
    cimg_forXY(img, x, y) {
      if (img(x, y, 0) == from[0] && img(x, y, 1) == from[1] &&
          img(x, y, 2) == from[2]) {
        img(x, y, 0) = to[0];
        img(x, y, 1) = to[1];
        img(x, y, 2) = to[2];
      }
    }
    return *this;
  }

  /**
   * @brief 画等边三角形
   *
   * @param x0 三角形中心的 x 坐标
   * @param y0 三角形中心的 y 坐标
   * @param length 三角形边长
   * @param color 填充的颜色
   * @return MyCImg& 返回当前对象
   */
  MyCImg& drawRegularTriangle(const int x0, const int y0, const int length,
                              const unsigned char* const color) {
    cimg_forXY(img, x, y) {
      // 进行坐标变换
      double xx = (double)x - (double)x0;
      double yy = (double)y - (double)y0 + 20.0 * sqrt3 / 3.0;
      double half = (double)length / 2.0;
      // 利用线性规划来划分出等边三角形区域
      if (xx >= -half && xx <= half && yy >= 0 && yy <= f1(xx) &&
          yy <= f2(xx)) {
        img(x, y, 0) = color[0];
        img(x, y, 1) = color[1];
        img(x, y, 2) = color[2];
      }
    }
    return *this;
  }

  /**
   * @brief 画圆形
   *
   * @param x0 圆心的 x 坐标
   * @param y0 圆心的 y 坐标
   * @param radius 圆的半径
   * @param color 填充的颜色
   * @return MyCImg& 返回当前对象
   */
  MyCImg& drawCircle(const int x0, const int y0, const int radius,
                     const unsigned char* const color) {
    cimg_forXY(img, x, y) {
      double distance = sqrt(pow(x - x0, 2) + pow(y - y0, 2));
      if (distance <= radius) {
        img(x, y, 0) = color[0];
        img(x, y, 1) = color[1];
        img(x, y, 2) = color[2];
      }
    }
    return *this;
  }

  /**
   * @brief 画线段
   *
   * @param x0 线段起点的 x 坐标
   * @param y0 线段起点的 y 坐标
   * @param tan 线段与 x 轴形成的角度的正切值
   * @param length 线段的长度
   * @param color 线段的颜色
   * @return MyCImg& 返回当前对象
   */
  MyCImg& drawLine(const int x0, const int y0, const double tan,
                   const int length, const unsigned char* const color) {
    cimg_forXY(img, x, y) {
      double distance = sqrt(pow(x - x0, 2) + pow(y - y0, 2));
      if (y == x * tan && distance <= length) {
        img(x, y, 0) = color[0];
        img(x, y, 1) = color[1];
        img(x, y, 2) = color[2];
      }
    }
    return *this;
  }

  /**
   * @brief 保存图像
   *
   * @param filename 保存的文件名
   * @return const MyCImg& 返回当前对象
   */
  const MyCImg& save(const char* const filename) const {
    img.save(filename);
    return *this;
  }

 private:
  CImg<unsigned char>& img;
};