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

class Hough {
public:
	Hough(string path);
	void edge_detect(double sigma, double tlow, double thigh);
	void generate_hough_space(double tgrad);
	void voting_line(double tvote, double peakdis);
	void get_line_equation();
	void draw_lines();
	void draw_edge();
	void draw_vertexs();
	void get_vertexs();
	void hough_process(double sigma, double tlow, double thigh, double tgrad,
		double tvote, double peakdis);

private:
	string path;
	CImg<int> hough_space;
	CImg<unsigned char> img;
	CImg<unsigned char> img_src;
	vector<Point> peaks;
	vector<Line> lines;
	vector<Point> vertexs;
};
