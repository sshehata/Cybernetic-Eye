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

void cleanPoints(Mat& image, vector< KeyPoint >& keypoints, int curv_thr) {
  for (int i = 0; i < keypoints.size(); ++i) {
    if(keypoints.at(i).response < RESPONSE_THRESHOLD) {
      keypoints.erase (keypoints.begin()+i);
      i--;
    }
  }

  // Second derivative kernels
  Mat xx = (cv::Mat_<unsigned char>(1,3) << 1, -2, 1);
  Mat yy = (cv::Mat_<unsigned char>(3,1) << 1, -2, 1);
  Mat xy = (cv::Mat_<unsigned char>(3,3) << 1, 0, -1, 0, 0, 0, -1, 0, 1);
  xy /= 4;

  // The matrices to contain the second derivatives
  Mat Dxx;
  Mat Dyy;
  Mat Dxy;
  conv<int>(xx, image, CONV_IGNORE_EDGE, Dxx);
  conv<int>(xy, image, CONV_IGNORE_EDGE, Dxy);
  conv<int>(yy, image, CONV_IGNORE_EDGE, Dyy);

  //  The matrices used for operations to check whether a point is clean or not
  Mat trace = Dxx + Dyy;
  Mat trace_squared = trace ^ 2;
  Mat Det = Dxx * Dyy - (Dxy) ^ 2;
  // trr is the trace squared * r
  Mat trr = trace_squared * curv_thr;
  // detr is the det 8 (r + 1) ^ 2
  Mat detr =  Det * (curv_thr + 1) ^ 2;

  // Loop responsible for removing points with low
  for (int i = 0; i < keypoints.size(); ++i) {
    KeyPoint point = keypoints.at(i);
    int octave = point.octave;
    int factor = 2 * octave;
    int row_index = (int)point.pt.x * factor;
    int col_index = (int)point.pt.y * factor;

    // the first condition checks for negative determinant which is rejected
    // and the second condition checks the ratio found in the paper
    if(Det.at<double>(row_index, col_index) < 0 ||
      trr.at<double>(row_index, col_index) > detr.at<double>(row_index, col_index)) {
      keypoints.erase (keypoints.begin()+i);
      i--;
      continue;
    }
  }
}