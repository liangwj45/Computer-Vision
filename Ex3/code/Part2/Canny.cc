#include "Canny.h"

#define M_PI 3.14159265358979323846
#define BOOSTBLURFACTOR 90.0
#define NOEDGE 255
#define POSSIBLE_EDGE 128
#define EDGE 0
#define IMGPATH "./img/"

using namespace std;
using namespace cimg_library;

static int black[] = { 0, 0, 0 };

Canny::Canny(string img_name) : img_name(img_name) {
	img = CImg<unsigned char>((IMGPATH + img_name).c_str());
	rows = img.width();
	cols = img.height();

	delta_x = new int[rows * cols];
	delta_y = new int[rows * cols];
	dir_radians = new float[rows * cols];
	magnitude = new int[rows * cols];
	smoothedim = new int[rows * cols];
	nms = new int[rows * cols];
	edge = new int[rows * cols];
	lconnect = new int[rows * cols];
	ldelete = new int[rows * cols];

	memset(delta_x, 0, rows * cols * sizeof(int));
	memset(delta_y, 0, rows * cols * sizeof(int));
	memset(dir_radians, 0, rows * cols * sizeof(float));
	memset(magnitude, 0, rows * cols * sizeof(int));
	memset(smoothedim, 0, rows * cols * sizeof(int));
	memset(nms, 0, rows * cols * sizeof(int));
	memset(edge, 0, rows * cols * sizeof(int));
	memset(lconnect, 0, rows * cols * sizeof(int));
	memset(ldelete, 0, rows * cols * sizeof(int));
}

Canny::~Canny() {
	delete[] delta_x;
	delete[] delta_y;
	delete[] dir_radians;
	delete[] magnitude;
	delete[] nms;
	delete[] edge;
	delete[] lconnect;
	delete[] ldelete;
}

CImg<unsigned char> Canny::canny_edge_detection(float sigma, float tlow,
	float thigh) {
	RGB2Gray();
	gaussian_smooth(sigma);
	derrivative_x_y();
	magnitude_x_y();
	radian_direction(-1, -1);
	non_max_supp();
	apply_hysteresis(tlow, thigh);
	return matrix2image(edge);
}

void Canny::RGB2Gray() {
	int r = 0, g = 0, b = 0;
	cimg_forXY(img, x, y) {
		r = img(x, y, 0);
		g = img(x, y, 1);
		b = img(x, y, 2);
		img(x, y, 0) = img(x, y, 1) = img(x, y, 2) = (r * 0.2 + g * 0.7 + b * 0.1);
	}
	img.resize(rows, cols, 1, 1, 5);
}

void Canny::gaussian_smooth(float sigma) {
	int r, c, rr, cc, /* Counter variables. */
		windowsize,   /* Dimension of the gaussian kernel. */
		center;       /* Half of the windowsize. */
	float* tempim,    /* Buffer for separable filter gaussian smoothing. */
		* kernel,      /* A one dimensional gaussian kernel. */
		dot,          /* Dot product summing variable. */
		sum;          /* Sum of the kernel weights variable. */

	tempim = new float[rows * cols];

	make_gaussian_kernel(sigma, &kernel, &windowsize);
	center = windowsize / 2;

	for (r = 0; r < rows; r++) {
		for (c = 0; c < cols; c++) {
			dot = 0.0;
			sum = 0.0;
			for (cc = (-center); cc <= center; cc++) {
				if (((c + cc) >= 0) && ((c + cc) < cols)) {
					dot += (float)img(r, c + cc) * kernel[center + cc];
					sum += kernel[center + cc];
				}
			}
			tempim[r * cols + c] = dot / sum;
		}
	}

	for (c = 0; c < cols; c++) {
		for (r = 0; r < rows; r++) {
			sum = 0.0;
			dot = 0.0;
			for (rr = (-center); rr <= center; rr++) {
				if (((r + rr) >= 0) && ((r + rr) < rows)) {
					dot += tempim[(r + rr) * cols + c] * kernel[center + rr];
					sum += kernel[center + rr];
				}
			}
			smoothedim[r * cols + c] = (short int)(dot * BOOSTBLURFACTOR / sum + 0.5);
		}
	}
}

void Canny::make_gaussian_kernel(float sigma, float** kernel, int* windowsize) {
	int i, center;
	float x, fx, sum = 0.0;

	*windowsize = 1 + 2 * ceil(2.5 * sigma);
	*kernel = new float[*windowsize];
	center = (*windowsize) / 2;

	for (i = 0; i < (*windowsize); i++) {
		x = (float)(i - center);
		fx = pow(2.71828, -0.5 * x * x / (sigma * sigma)) /
			(sigma * sqrt(6.2831853));
		(*kernel)[i] = fx;
		sum += fx;
	}

	for (i = 0; i < (*windowsize); i++) (*kernel)[i] /= sum;
}

void Canny::derrivative_x_y() {
	int r, c, pos;

	for (r = 0; r < rows; r++) {
		pos = r * cols;
		delta_x[pos] = smoothedim[pos + 1] - smoothedim[pos];
		pos++;
		for (c = 1; c < (cols - 1); c++, pos++) {
			delta_x[pos] = smoothedim[pos + 1] - smoothedim[pos - 1];
		}
		delta_x[pos] = smoothedim[pos] - smoothedim[pos - 1];
	}

	for (c = 0; c < cols; c++) {
		pos = c;
		delta_y[pos] = smoothedim[pos + cols] - smoothedim[pos];
		pos += cols;
		for (r = 1; r < (rows - 1); r++, pos += cols) {
			delta_y[pos] = smoothedim[pos + cols] - smoothedim[pos - cols];
		}
		delta_y[pos] = smoothedim[pos] - smoothedim[pos - cols];
	}
}

void Canny::magnitude_x_y() {
	int r = 0, c = 0, pos = 0, sq1 = 0, sq2 = 0;

	for (r = 0, pos = 0; r < rows; r++) {
		for (c = 0; c < cols; c++, pos++) {
			sq1 = (int)delta_x[pos] * (int)delta_x[pos];
			sq2 = (int)delta_y[pos] * (int)delta_y[pos];
			magnitude[pos] = (short)(0.5 + sqrt((float)sq1 + (float)sq2));
		}
	}
}

void Canny::radian_direction(int xdirtag, int ydirtag) {
	double dx = 0.0, dy = 0.0;
	int r = 0, c = 0, pos = 0;

	for (r = 0, pos = 0; r < rows; r++) {
		for (c = 0; c < cols; c++, pos++) {
			dx = (double)delta_x[pos];
			dy = (double)delta_y[pos];

			if (xdirtag == 1) dx = -dx;
			if (ydirtag == -1) dy = -dy;

			dir_radians[pos] = (float)angle_radians(dx, dy);
		}
	}
}

double Canny::angle_radians(double x, double y) {
	double xu = 0.0, yu = 0.0, ang = 0.0;

	xu = fabs(x);
	yu = fabs(y);

	if ((xu == 0) && (yu == 0)) return (0);

	ang = atan(yu / xu);

	if (x >= 0) {
		if (y >= 0)
			return (ang);
		else
			return (2 * M_PI - ang);
	}
	else {
		if (y >= 0)
			return (M_PI - ang);
		else
			return (M_PI + ang);
	}
}

void Canny::non_max_supp() {
	int rowcount = 0, colcount = 0, count = 0;
	int* magrowptr, * magptr;
	int* gxrowptr, * gxptr;
	int* gyrowptr, * gyptr, z1 = 0, z2 = 0;
	int m00 = 0, gx = 0, gy = 0;
	float mag1 = 0.0, mag2 = 0.0, xperp = 0.0, yperp = 0.0;
	int* resultrowptr, * resultptr;

	/****************************************************************************
	 * Zero the edges of the result image.
	 ****************************************************************************/
	for (count = 0, resultrowptr = nms, resultptr = nms + cols * (rows - 1);
		count < cols; resultptr++, resultrowptr++, count++) {
		*resultrowptr = *resultptr = (unsigned char)0;
	}

	for (count = 0, resultptr = nms, resultrowptr = nms + cols - 1; count < rows;
		count++, resultptr += cols, resultrowptr += cols) {
		*resultptr = *resultrowptr = (unsigned char)0;
	}

	/****************************************************************************
	 * Suppress non-maximum points.
	 ****************************************************************************/
	for (rowcount = 1, magrowptr = magnitude + cols + 1,
		gxrowptr = delta_x + cols + 1, gyrowptr = delta_y + cols + 1,
		resultrowptr = nms + cols + 1;
		rowcount < rows - 2; rowcount++, magrowptr += cols, gyrowptr += cols,
		gxrowptr += cols, resultrowptr += cols) {
		for (colcount = 1, magptr = magrowptr, gxptr = gxrowptr, gyptr = gyrowptr,
			resultptr = resultrowptr;
			colcount < cols - 2;
			colcount++, magptr++, gxptr++, gyptr++, resultptr++) {
			m00 = *magptr;
			if (m00 == 0) {
				*resultptr = NOEDGE;
			}
			else {
				xperp = -(gx = *gxptr) / ((float)m00);
				yperp = (gy = *gyptr) / ((float)m00);
			}

			if (gx >= 0) {
				if (gy >= 0) {
					if (gx >= gy) {
						/* 111 */
						/* Left point */
						z1 = *(magptr - 1);
						z2 = *(magptr - cols - 1);

						mag1 = (m00 - z1) * xperp + (z2 - z1) * yperp;

						/* Right point */
						z1 = *(magptr + 1);
						z2 = *(magptr + cols + 1);

						mag2 = (m00 - z1) * xperp + (z2 - z1) * yperp;
					}
					else {
						/* 110 */
						/* Left point */
						z1 = *(magptr - cols);
						z2 = *(magptr - cols - 1);

						mag1 = (z1 - z2) * xperp + (z1 - m00) * yperp;

						/* Right point */
						z1 = *(magptr + cols);
						z2 = *(magptr + cols + 1);

						mag2 = (z1 - z2) * xperp + (z1 - m00) * yperp;
					}
				}
				else {
					if (gx >= -gy) {
						/* 101 */
						/* Left point */
						z1 = *(magptr - 1);
						z2 = *(magptr + cols - 1);

						mag1 = (m00 - z1) * xperp + (z1 - z2) * yperp;

						/* Right point */
						z1 = *(magptr + 1);
						z2 = *(magptr - cols + 1);

						mag2 = (m00 - z1) * xperp + (z1 - z2) * yperp;
					}
					else {
						/* 100 */
						/* Left point */
						z1 = *(magptr + cols);
						z2 = *(magptr + cols - 1);

						mag1 = (z1 - z2) * xperp + (m00 - z1) * yperp;

						/* Right point */
						z1 = *(magptr - cols);
						z2 = *(magptr - cols + 1);

						mag2 = (z1 - z2) * xperp + (m00 - z1) * yperp;
					}
				}
			}
			else {
				if ((gy = *gyptr) >= 0) {
					if (-gx >= gy) {
						/* 011 */
						/* Left point */
						z1 = *(magptr + 1);
						z2 = *(magptr - cols + 1);

						mag1 = (z1 - m00) * xperp + (z2 - z1) * yperp;

						/* Right point */
						z1 = *(magptr - 1);
						z2 = *(magptr + cols - 1);

						mag2 = (z1 - m00) * xperp + (z2 - z1) * yperp;
					}
					else {
						/* 010 */
						/* Left point */
						z1 = *(magptr - cols);
						z2 = *(magptr - cols + 1);

						mag1 = (z2 - z1) * xperp + (z1 - m00) * yperp;

						/* Right point */
						z1 = *(magptr + cols);
						z2 = *(magptr + cols - 1);

						mag2 = (z2 - z1) * xperp + (z1 - m00) * yperp;
					}
				}
				else {
					if (-gx > -gy) {
						/* 001 */
						/* Left point */
						z1 = *(magptr + 1);
						z2 = *(magptr + cols + 1);

						mag1 = (z1 - m00) * xperp + (z1 - z2) * yperp;

						/* Right point */
						z1 = *(magptr - 1);
						z2 = *(magptr - cols - 1);

						mag2 = (z1 - m00) * xperp + (z1 - z2) * yperp;
					}
					else {
						/* 000 */
						/* Left point */
						z1 = *(magptr + cols);
						z2 = *(magptr + cols + 1);

						mag1 = (z2 - z1) * xperp + (m00 - z1) * yperp;

						/* Right point */
						z1 = *(magptr - cols);
						z2 = *(magptr - cols - 1);

						mag2 = (z2 - z1) * xperp + (m00 - z1) * yperp;
					}
				}
			}

			/* Now determine if the current point is a maximum point */

			if ((mag1 > 0.0) || (mag2 > 0.0)) {
				*resultptr = NOEDGE;
			}
			else {
				if (mag2 == 0.0)
					* resultptr = NOEDGE;
				else
					*resultptr = POSSIBLE_EDGE;
			}
		}
	}
}

void Canny::apply_hysteresis(float tlow, float thigh) {
	int r = 0, c = 0, pos = 0, numedges = 0, lowcount = 0, highcount = 0,
		lowthreshold = 0, highthreshold = 0, i = 0, hist[32768], rr = 0, cc = 0;
	short int maximum_mag = 0, sumpix = 0;

	for (r = 0, pos = 0; r < rows; r++) {
		for (c = 0; c < cols; c++, pos++) {
			if (nms[pos] == POSSIBLE_EDGE)
				edge[pos] = POSSIBLE_EDGE;
			else
				edge[pos] = NOEDGE;
		}
	}

	for (r = 0, pos = 0; r < rows; r++, pos += cols) {
		edge[pos] = NOEDGE;
		edge[pos + cols - 1] = NOEDGE;
	}

	pos = (rows - 1) * cols;

	for (c = 0; c < cols; c++, pos++) {
		edge[c] = NOEDGE;
		edge[pos] = NOEDGE;
	}

	for (r = 0; r < 32768; r++) hist[r] = 0;
	for (r = 0, pos = 0; r < rows; r++) {
		for (c = 0; c < cols; c++, pos++) {
			if (edge[pos] == POSSIBLE_EDGE) hist[magnitude[pos]]++;
		}
	}

	for (r = 1, numedges = 0; r < 32768; r++) {
		if (hist[r] != 0) maximum_mag = r;
		numedges += hist[r];
	}

	highcount = (int)(numedges * thigh + 0.5);

	r = 1;
	numedges = hist[1];

	while ((r < (maximum_mag - 1)) && (numedges < highcount)) {
		r++;
		numedges += hist[r];
	}

	highthreshold = r;
	lowthreshold = (int)(highthreshold * tlow + 0.5);

	for (r = 0, pos = 0; r < rows; r++) {
		for (c = 0; c < cols; c++, pos++) {
			if ((edge[pos] == POSSIBLE_EDGE) && (magnitude[pos] >= highthreshold)) {
				edge[pos] = EDGE;
				follow_edges((edge + pos), (magnitude + pos), lowthreshold, cols);
			}
		}
	}

	for (r = 0, pos = 0; r < rows; r++) {
		for (c = 0; c < cols; c++, pos++)
			if (edge[pos] != EDGE) edge[pos] = NOEDGE;
	}
}

void Canny::follow_edges(int* edgemapptr, int* edgemagptr, int lowval,
	int cols) {
	int* tempmagptr;
	int* tempmapptr;
	int i;
	float thethresh;
	int x[8] = { 1, 1, 0, -1, -1, -1, 0, 1 }, y[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };

	for (i = 0; i < 8; i++) {
		tempmapptr = edgemapptr - y[i] * cols + x[i];
		tempmagptr = edgemagptr - y[i] * cols + x[i];

		if ((*tempmapptr == POSSIBLE_EDGE) && (*tempmagptr > lowval)) {
			*tempmapptr = EDGE;
			follow_edges(tempmapptr, tempmagptr, lowval, cols);
		}
	}
}

#include <iostream>

void Canny::save_result() {
	string path = "./result/" + this->img_name + "edge-";
	std::cout << path << std::endl;
	matrix2image(edge).save(path.c_str());
}

CImg<unsigned char> Canny::matrix2image(int* matrix) {
	CImg<unsigned char> img(rows, cols, 1, 1, 5);
	img.fill(0);
	cimg_forXY(img, x, y) { img(x, y, 0) = matrix[x * cols + y]; }
	return img;
}

void Canny::image2matrix(CImg<unsigned char> img, int* matrix) {
	cimg_forXY(img, x, y) { matrix[x * cols + y] = img(x, y, 0); }
}
