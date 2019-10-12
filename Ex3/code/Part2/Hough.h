#include "CImg.h"
#include "Canny.h"

#include <vector>
using namespace cimg_library;
using std::pair;
using std::vector;

struct Point {
	int x, y, value;
};

struct Line {
	double a, b, c;
};

struct Circle {
	int x, y, r, value;
};

class Hough {
public:
	Hough(string path);
	void hough_process(double sigma, double tlow, double thigh, double tgrad,
		int minR, int maxR, double tvote, int center_dis);
	void edge_detect(double sigma, double tlow, double thigh);
	void vote_centers(double tgrad, double tvote, int minR, int maxR,
		int center_dis);
	void draw_circles();

private:
	string path;
	CImg<int> hough_space;
	CImg<unsigned char> img;
	CImg<unsigned char> img_src;
	vector<double> sin_set;
	vector<double> cos_set;
	vector<Circle> circles;
};
