#define _CRT_SECURE_NO_WARNINGS
#include "Identify.h"

Identify::Identify(bool has_model)
    : training_vectors(num_images * 20, img_size, CV_32F),
      training_labels(num_images * 20, 1, CV_32F)
#if Debug
      ,
      testing_images(num_test, img_size, CV_32F),
      testing_result(num_test, 1, CV_32F)
#endif
{
  knn = ml::KNearest::create();
  if (has_model) {
    ImportTrainingModel();
  } else {
    LoadDataAndDoTraining();
  }

#if Debug
  ImportTestingImages();
  DoTesting();
#endif
}

void Identify::Predict(Mat& test, Mat& result) {
  knn->findNearest(test, K, result);
}

float Identify::Predict(string path) {
  Mat result(1, 1, CV_32F);
  Mat test(1, img_size, CV_32F);
  Mat img = imread(path, IMREAD_GRAYSCALE);
  img.convertTo(img, CV_32F);
  resize(img, img, Size(num_rows, num_cols));
  for (int i = 0; i < img_size; i++) {
    test.at<float>(0, i) = img.at<float>(i / num_cols, i % num_cols);
  }
  knn->findNearest(test, K, result);
  return (int)result.at<float>(0, 0);
}

float Identify::Predict(CImg<unsigned char>& img) {
  img.resize(num_rows, num_cols, 1, 1);
  img.save("tmp_image_for_predict.bmp");
  return Predict("tmp_image_for_predict.bmp");
}

void Identify::ImportTrainingModel() {
  // knn = Algorithm::load<ml::KNearest>("identify.model");
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
  knn->setDefaultK(K);
  knn->setIsClassifier(true);
  knn->setAlgorithmType(ml::KNearest::BRUTE_FORCE);
  knn->train(training_vectors, ml::ROW_SAMPLE, training_labels);
}

void Identify::LoadDataAndDoTraining() {
  ofstream image("training.images");
  ofstream label("training.labels");
  for (int i = 0; i <= 19; i++) {
    for (int j = 0; j < num_images; j++) {
      char buf[1024];
      sprintf(buf, "dataset/%d/%d.png", i, j);
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
  knn->setDefaultK(K);
  knn->setIsClassifier(true);
  knn->setAlgorithmType(ml::KNearest::BRUTE_FORCE);
  knn->train(training_vectors, ml::ROW_SAMPLE, training_labels);
  // knn->save("identify.model");
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
