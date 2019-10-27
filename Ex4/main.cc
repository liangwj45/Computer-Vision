#include "Ex4.h"
#include <iostream>
using namespace cimg_library;

int main() {
	Ex4 seg("./img/1.bmp");
	seg.test();
}


//#include <iostream>  
//#include <opencv2/opencv.hpp>
//#include <opencv2/highgui/highgui_c.h>
//
//using namespace std;
//using namespace cv;
//
//int main() {
//	//Mat src;
//	//src = imread("./img/1.bmp");
//	//resize(src, src, Size(src.cols / 2, src.rows / 2));
//
//	//Mat src_gray;
//	//cvtColor(src, src_gray, CV_WINDOW_AUTOSIZE);
//
//	//Mat src_canny;
//	//Canny(src_gray, src_canny, 120, 200);
//
//	//vector<Vec4f> line_data;
//	//HoughLinesP(src_canny, line_data, 1, CV_PI / 180.0, 80, 10, 20);
//
//	//Scalar blue = Scalar(255, 0, 0);
//	//Vec4f temp = line_data[0];
//	//line(src, Point(temp[0], temp[1]), Point(temp[2], temp[3]), blue, 2);
//
//	//imshow("houghLinesP img", src);
//
//	Mat srcImage = imread("./img/1.bmp");
//	resize(srcImage, srcImage, Size(srcImage.cols / 2, srcImage.rows / 2));
//	imshow("原图", srcImage);
//	//对图像进行处理，转化为灰度图然后再转为二值图
//	Mat grayImage;
//	cvtColor(srcImage, grayImage, COLOR_BGR2GRAY);
//	Mat binImage;
//	//第4个参数为CV_THRESH_BINARY_INV是因为我的输入原图为白底黑字
//	//若为黑底白字则选择CV_THRESH_BINARY即可
//	threshold(grayImage, binImage, 100, 255, CV_THRESH_BINARY_INV);
//
//	Mat conImage = Mat::zeros(binImage.size(), binImage.type());
//	vector<vector<Point>> contours;
//	vector<Vec4i> hierarchy;
//	//指定CV_RETR_EXTERNAL寻找数字的外轮廓
//	findContours(binImage, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
//	//绘制轮廓
//	drawContours(conImage, contours, -1, 255);
//	imshow("houghLinesP img", conImage);
//
//	waitKey(0);
//	return 0;
//}
