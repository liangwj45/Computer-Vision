#define _CRT_SECURE_NO_WARNINGS
#include "Identify.h"

Identify::Identify(bool has_image)
    : training_vectors(num_images * 20, img_size, CV_32F),
      training_labels(num_images * 20, 1, CV_32F)
#if Debug
      ,
      testing_images(num_test, img_size, CV_32FC1),
      testing_result(num_test, 1, CV_32F)
#endif
{
  if (has_image) {
    LoadImagesAndLabels();
  } else {
    ImportTrainingImages();
  }
  knn = ml::KNearest::create();
  DoTraining();

#if Debug
  ImportTestingImages();
  DoTesting();
#endif
}

void Identify::Predict(Mat& test, Mat& result) {
  knn->findNearest(test, K, result);
}

float Identify::Predict(Mat& test) {
  Mat result(1, 1, CV_32F);
  knn->findNearest(test, K, result);
  return result.at<float>(0, 0);
}

void Identify::ImportTrainingImages() {
  ofstream image("training.images");
  ofstream label("training.labels");
  for (int i = 0; i <= 19; i++) {
    for (int j = 0; j < num_images; j++) {
      char buf[1024];
      sprintf(buf, "num/tmp/%d/%d.png", i, j);
      Mat img = imread(buf, IMREAD_GRAYSCALE);
      img.convertTo(img, CV_32F);
      resize(img, img, Size(num_rows, num_cols));
      int index = i * num_images + j;
      for (int k = 0; k < img_size; k++) {
        training_vectors.at<float>(index, k) =
            img.at<float>(k / num_cols, k % num_cols);
        image << training_vectors.at<float>(index, k) << std::endl;
      }
      training_labels.at<float>(index, 0) = i;
      label << i << std::endl;
    }
  }
}

void Identify::LoadImagesAndLabels() {
  ifstream image("training.images");
  ifstream label("training.labels");
  float n;
  for (int i = 0; i <= 19; i++) {
    for (int j = 0; j < num_images; j++) {
      int index = i * num_images + j;
      for (int k = 0; k < img_size; k++) {
        image >> n;
        training_vectors.at<float>(index, k) = n;
      }
      label >> n;
      training_labels.at<float>(index, 0) = n;
    }
  }
}

void Identify::DoTraining() {
  knn->setDefaultK(K);
  knn->setIsClassifier(true);
  knn->setAlgorithmType(ml::KNearest::BRUTE_FORCE);
  knn->train(training_vectors, ml::ROW_SAMPLE, training_labels);
}

#if Debug
#include <iostream>
using std::cout;
using std::endl;

void Identify::ImportTestingImages() {
  for (int i = 0; i < num_test; i++) {
    char buf[1024];
    sprintf(buf, "tmp/%d.bmp", i);
    Mat img = imread(buf, IMREAD_GRAYSCALE);
    img.convertTo(img, CV_32F);
    resize(img, img, Size(num_rows, num_cols));
    for (int j = 0; j < img_size; j++) {
      testing_images.at<float>(i, j) =
          img.at<float>(j / num_cols, j % num_cols);
    }
  }
}

void Identify::DoTesting() {
  knn->findNearest(testing_images, K, testing_result);
  int cnt = 0;
  for (int k = 0; k < num_test; k++) {
    cout << k << " " << testing_result.at<float>(k) << endl;
    if (k == testing_result.at<float>(k)) {
      cnt++;
    }
  }
  cout << "K: " << K << "  correct: " << cnt << endl;
}
#endif