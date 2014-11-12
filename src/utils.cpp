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

using cv::Mat;

template <typename T>
void Conv(const Mat& kernel, const cv::Mat& signal,
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

template void Conv<int>(const cv::Mat&, const cv::Mat&, const uchar, cv::Mat&);

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
