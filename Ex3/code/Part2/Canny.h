#pragma
#include <cmath>
#include <cstring>
#include <string>

#include "CImg.h"

using namespace std;
using namespace cimg_library;

class Canny {
private:
	CImg<unsigned char> img;
	string img_name;     // The name of the image.
	int rows;            // The height of the image.
	int cols;            // The width of the image.
	int* delta_x;        // The first devivative image, x-direction.
	int* delta_y;        // The first derivative image, y-direction.
	float* dir_radians;  // Gradient direction image.
	int* magnitude;      // The magnitude of the gadient image.
	int* smoothedim;     // The image after gaussian smoothing.
	int* nms;            // The image after non-maximize suppression.
	int* edge;           // The image after hysterresis.
	int* lconnect;       // The image after connecting line.
	int* ldelete;        // The image after deleting line.
public:
	Canny(string img_name);
	~Canny();

	CImg<unsigned char> canny_edge_detection(float sigma, float tlow,
		float thigh);
	void save_result();

	void RGB2Gray();
	void gaussian_smooth(float sigma);
	void make_gaussian_kernel(float sigma, float** kernel, int* windowsize);
	void derrivative_x_y();
	void magnitude_x_y();
	void radian_direction(int xdirtag, int ydirtag);
	double angle_radians(double x, double y);
	void non_max_supp();
	void apply_hysteresis(float tlow, float thigh);
	void follow_edges(int* edgemapptr, int* edgemagptr, int lowval, int cols);

	CImg<unsigned char> matrix2image(int* matrix);
	void image2matrix(CImg<unsigned char> img, int* matrix);
};
