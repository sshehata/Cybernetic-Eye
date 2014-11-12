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
 *         Author:  Samy Shihata (sshihata), Mohamed Ashraf (m0hamed)
 *   Organization:  GUC
 *
 * =====================================================================================
 */

#include "utils.h"
#include <cmath>

using cv::Mat;
using cv::Size;
using cv::Ptr;
using std::vector;

template <typename T>
void Conv(const cv::Mat& kernel, const cv::Mat& signal,
    const uchar edge_handling, cv::Mat& output) {
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

template void Conv<int>(const cv::Mat&, const cv::Mat&, const uchar, cv::Mat&);

template <typename T>
void buildGaussianPyramid(const Mat& image, vector< vector <Mat> >& pyr,
    int nOctaves) {
  int nScales = 5;
  double sigma = sqrt(2);
  for(int j=0; j<nOctaves; j++) {
    for(int i=0; i<nScales; i++) {
      if(i == 0 && j == 0) {
        pyr[0][0] = image;
      } else if (i == 0) {
        pyr[0][j] = downSample(pyr[1][j-1]);
      } else {
        Ptr<cv::FilterEngine> e = cv::createGaussianFilter(image.type(),
            Size(3, 3), sigma *pow(sqrt(2), i));
        e->apply(pyr[i-1][j], pyr[i][j]);
      }
    }
  }

}

template void buildGaussianPyramid<int>(const Mat&, vector< vector <Mat> >&, int);
