#ifndef _Ex5_Identify_
#define _Ex5_Identify_
#define Debug 0
#include <fstream>
#include <string>
#include "CImg.h"
#include "opencv2/opencv.hpp"
using namespace cimg_library;
using namespace cv;
using std::ifstream;
using std::ofstream;

class Identify {
 public:
  Identify(bool has_image = false);
  void Predict(Mat& test, Mat& result);
  float Predict(Mat& test);

 private:
  void ImportTrainingImages();
  void LoadImagesAndLabels();
  void DoTraining();

#if Debug
  void ImportTestingImages();
  void DoTesting();
#endif

 private:
  int K = 10;
  const int num_images = 1016;
  const int num_cols = 24;
  const int num_rows = 24;
  const int img_size = num_cols * num_rows;
  Ptr<ml::KNearest> knn;
  Mat training_vectors;
  Mat training_labels;

#if Debug
  const int num_test = 10;
  Mat testing_images;
  Mat testing_result;
#endif
};

#endif