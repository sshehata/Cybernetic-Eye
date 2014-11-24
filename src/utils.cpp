/*
 * =====================================================================================
 *
 *       Filename:  utils.cpp
 *
 *    Description:  A collection of math utility functions.
 *
 *        Version:  1.0
 *        Created:  10/09/2014 02:42:01 AM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Samy Shihata (sshihata), Mohamed Ashraf (m0hamed), Hossam Ahmed (hoss93)
 *   Organization:  GUC
 *
 * =====================================================================================
 */

#include "utils.h"
#include "globals.h"
#include <cmath>

using cv::Mat;
using cv::Size;
using cv::Ptr;
using std::vector;
using std::cout;
using std::endl;

template <typename T>
void conv(const Mat& kernel, const cv::Mat& signal,
    const uchar edge_handling, Mat& output) {
  int i, startj, limiti, limitj;
  int half_rows = kernel.rows / 2;
  int half_cols = kernel.cols / 2;
  switch (edge_handling) {
    case CONV_IGNORE_EDGE:
      i = half_rows;
      startj = half_cols;
      limiti = half_rows;
      limitj = half_cols;
      break;
  }
  for (;i < signal.rows - limiti;i++) {
    for (int j = startj; j < signal.cols - limitj; j++) {
      output.at<T>(i, j) = kernel.dot(signal(cv::Rect(j - half_cols, i - half_rows,
          kernel.rows, kernel.cols)));
    }
  }
}

template void conv<int>(const cv::Mat&, const cv::Mat&, const uchar, cv::Mat&);

template <typename T>
Mat downSample(const Mat& image) {
  int half_rows = image.rows / 2;
  int half_cols = image.cols / 2;
  Mat output(half_rows, half_cols, image.type());
  for (int i=0; i<half_rows; i++) {
    for(int j=0; j<half_cols; j++) {
      output.at<T>(i, j) = image.at<T>(i*2, j*2);
    }
  }
  return output;
}

template Mat downSample<int>(const cv::Mat&);
template Mat downSample<uchar>(const cv::Mat&);
template Mat downSample<double>(const cv::Mat&);

template <typename T>
Mat upSample(const Mat& image) {
  int double_rows = image.rows * 2;
  int double_cols = image.cols * 2;
  Mat output(double_rows, double_cols, image.type());
  for (int i=0; i<double_rows; i++) {
    for(int j=0; j<double_cols; j++) {
      output.at<T>(i, j) = image.at<T>(i/2, j/2);
    }
  }
  return output;
}

template Mat upSample<int>(const cv::Mat&);
template Mat upSample<uchar>(const cv::Mat&);
template Mat upSample<double>(const cv::Mat&);

// pyr[i][j] is the image at octave i and scale j
template <typename T>
void buildGaussianPyramid(const Mat& image, vector< vector <Mat>>& pyr,
    int n_octaves) {
  int n_scales = SIFT_NUMBER_OF_SCALES;
  double sigma = SIFT_INITIAL_SIGMA;
  for(int i=0; i<n_octaves; i++) {
    pyr.push_back(vector<Mat>());
    for(int j=0; j<n_scales; j++) {
      if(i == 0 && j == 0) {
        pyr[0].push_back(image);
      } else if (j == 0) {
        pyr[i].push_back(downSample<T>(pyr[i-1][1]));
      } else {
        double new_sigma = (sigma * pow(SIFT_SIGMA_CHANGE, i));
        int filter_size = new_sigma*6;
        filter_size += 1 - filter_size %2;
        Ptr<cv::FilterEngine> e = cv::createGaussianFilter(image.type(),
            Size(filter_size, filter_size), new_sigma);
        pyr[i].push_back(pyr[i][j-1].clone()); // to set the correct size and type
        e->apply(pyr[i][j-1], pyr[i][j]);
      }
    }
  }
}

template void buildGaussianPyramid<int>(const Mat&, vector< vector <Mat> >&, int);
template void buildGaussianPyramid<uchar>(const Mat&, vector< vector <Mat> >&, int);
template void buildGaussianPyramid<double>(const Mat&, vector< vector <Mat> >&, int);

vector<vector<Mat>> buildDogPyramid(vector<vector<Mat>>& gauss_pyr) {
  vector<vector<Mat>> pyramid;
  int octaves = gauss_pyr.size();
  int scales = gauss_pyr[0].size();
  for(int i=0; i < octaves; i++) {
    pyramid.push_back(vector<Mat>());
    for(int j=0; j < scales - 1; j++) {
      pyramid[i].push_back(gauss_pyr[i][j] - gauss_pyr[i][j+1]);
    }
  }
  return pyramid;
}

template<typename T>
T getMax(const Mat image) {
  T max = -1;
  int maxi = 0;
  int maxj = 0;
  for (int i = 1; i < image.rows; i++) {
    for (int j = 1; j < image.cols; j++) {
      T pixel = image.at<T>(i, j);
      if(pixel > max) {
        max = pixel;
        maxi = i;
        maxj = j;
      }
    }
  }
  cout << maxi << " " << maxj << endl;
  return max;
}
template double getMax<double>(const Mat image);
template int getMax<int>(const Mat image);
