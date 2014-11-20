/*
 * =====================================================================================
 *
 *       Filename:  sift.cpp
 *
 *    Description:  functions specific to sift algorithm implementation
 *
 *        Version:  1.0
 *        Created:  11/14/2014 08:23:36 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Samy Shihata (sshihata), Hossam Ahmed (hoss93)
 *   Organization:  GUC
 *
 * =====================================================================================
 */

#include "sift.h"
#include "globals.h"
using namespace cv;
using std::vector;
using cv::Mat;
using cv::KeyPoint;
using cv::Rect;

template<typename T>
void getScaleSpaceExtrema(const vector< vector< Mat > >& pyr,
    vector< KeyPoint >& keypoints) {
  int octaves = (int) pyr.size();
  int scales = (int) pyr[0].size() - 1;
  for (int i = 0; i < octaves; i++)
    for (int j = 1; j < scales; j++) {
      vector< Mat > sample_scales;
      sample_scales.push_back(pyr[i][j]);
      sample_scales.push_back(pyr[i][j-1]);
      sample_scales.push_back(pyr[i][j+1]);
      getExtrema<T>(sample_scales, i, keypoints);
    }
}

template void getScaleSpaceExtrema<int>(const vector< vector< Mat > >&,
    vector< KeyPoint >&);
template void getScaleSpaceExtrema<uchar>(const vector< vector< Mat > >&,
    vector< KeyPoint >&);
template void getScaleSpaceExtrema<double>(const vector< vector< Mat > >&,
    vector< KeyPoint >&);


template <typename T>
inline bool isMinMax(const T pixel, const Rect& rect, const vector< Mat >& sample_scales) {
  bool not_min = false, not_max = false;
  for (int samples = 0; samples < (int)sample_scales.size(); samples++) {
    Mat window = sample_scales[samples](rect);
    for (int pi = 0; pi < window.rows; pi++) {
      for (int pj = 0; pj < window.cols; pj++) {
        T neig = window.at<T>(pi, pj);
        not_min = neig < pixel || not_min;
        not_max = neig > pixel || not_max;
        if (not_min && not_max)
          return false;
      }
    }
  }
  return true;
}

template<typename T>
void getExtrema(const vector< Mat >& sample_scales, const int octave,
    vector< KeyPoint >& keypoints) {
  for (int i = 1; i < sample_scales[0].rows-1; i++) {
    for (int j = 1; j < sample_scales[0].cols-1; j++) {
      T pixel = sample_scales[0].at<T>(i, j);
      Rect rect(j-1, i-1, 3, 3);
      if (isMinMax(pixel, rect, sample_scales)){
        keypoints.push_back(KeyPoint(j, i, 0,-1.0f, pixel, octave, -1));
      }
    }
  }
}

template void getExtrema<int>(const vector< Mat >&, const int, vector< KeyPoint >& );
template void getExtrema<uchar>(const vector< Mat >&, const int, vector< KeyPoint >& );
template void getExtrema<double>(const vector< Mat >&, const int, vector< KeyPoint >& );

vector< KeyPoint > cleanPoints(Mat& image, vector< KeyPoint >& keypoints) {
  vector<KeyPoint> valid_keypoints;
  // Second derivative kernels
  Mat xx = (Mat_<double>(1,3) << 1, -2, 1);
  Mat yy = (Mat_<double>(3,1) << 1, -2, 1);
  Mat xy = (Mat_<double>(3,3) << 1, 0, -1, 0, 0, 0, -1, 0, 1);
  xy /= 4;

  // The matrices to contain the second derivatives
  Mat Dxx;
  Mat Dyy;
  Mat Dxy;
  filter2D( image, Dxx, -1 , xx, Point( -1, -1 ), 0, BORDER_DEFAULT );
  filter2D( image, Dxy, -1 , xy, Point( -1, -1 ), 0, BORDER_DEFAULT );
  filter2D( image, Dyy, -1 , yy, Point( -1, -1 ), 0, BORDER_DEFAULT );

  //  The matrices used for operations to check whether a point is clean or not
  Mat trace = Dxx + Dyy;
  // This has to be element wise because it would have been Dxx * Dyy'
  Mat det = Dxx.mul(Dyy) - Dxy.mul(Dxy);
  for (int i = 0; i < keypoints.size(); ++i) {
    KeyPoint point = keypoints.at(i);
    int octave = point.octave;
    int factor = pow(2,octave);
    int row_index = (int)point.pt.y * factor;
    int col_index = (int)point.pt.x * factor;
    double principal_curvature = det.at<double>(row_index, col_index) -
          ALPHA * pow(trace.at<double>(row_index, col_index),2);
    double pixel_value = image.at<double>(row_index, col_index);
    if(principal_curvature < PRINCIPAL_CURVATURE_THRESHOLD && pixel_value > RESPONSE_THRESHOLD) {
      valid_keypoints.push_back(keypoints.at(i));
    }
  }
  return valid_keypoints;
}