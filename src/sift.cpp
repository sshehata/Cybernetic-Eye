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
 *         Author:  Samy Shihata (sshihata),
 *   Organization:  GUC
 *
 * =====================================================================================
 */

#include "sift.h"

using std::vector;
using cv::Mat;
using cv::KeyPoint;
using cv::Rect;
using cv::Point;
using cv::Sobel;
using cv::magnitude;

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

template <typename T>
vector< double > computeOrientationHist(const Mat& image, const KeyPoint& keypoint) {
  int x = keypoint.pt.x - SIFT_KEYPOINT_WND_WIDTH - 1;
  int y = keypoint.pt.y - SIFT_KEYPOINT_WND_HEIGHT - 1;
  int width = SIFT_KEYPOINT_WND_WIDTH + 2;
  int height = SIFT_KEYPOINT_WND_HEIGHT + 2;
  if (x < 0)
    x = 0;
  if (y < 0)
    y = 0;
  if (x + width > image.cols)
    width = image.cols - x;
  if (y + height > image.rows)
    height = image.rows - y;
  Rect rect(x, y, width, height);
  Mat src_wnd = image(rect);
  Mat dx, dy, mag, angle;
  Sobel(src_wnd, dx, src_wnd.depth(), 1, 0);
  Sobel(src_wnd, dy, src_wnd.depth(), 0, 1);
  magnitude(dx, dy, mag);
  phase(dx, dy, angle, true);
  vector<double> bins(128);
  for (int i = 1; i < src_wnd.rows - 1; i++) {
    for (int j = 1; j < src_wnd.cols - 1; j++) {
      int base_bin = ((((i - 1) / 4) * 4) + ((j - 1) / 4)) * 8;
      int bin = base_bin + (int)(angle.at<T>(i, j) / 45);
      bins[bin] += mag.at<T>(i, j);
    }
  }
  return bins;
}

template vector< double > computeOrientationHist<double>(const Mat&, const KeyPoint&);
