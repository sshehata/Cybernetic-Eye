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
#include "globals.h"

template<typename T>
void getScaleSpaceExtrema(const vector< vector< Mat > >& pyr, 
    vector< KeyPoint >& keypoints) {
  int octaves = (int) pyr.size();
  int scales = (int) pyr[0].size();
  for(int i = 0; i < octaves; i++)
    for(int j = 0; j < scales; j += SCALE_SAMPLE_FREQ * scales) {
      vector< Mat > sample_scales(3);
      sample_scales.push_back(pyr[i][j]);
      if(j > 0)
        sample_scales.push_back(pyr[i][j-1]);
      if(j < scales - 1)
        sample_scales.push_back(pyr[i][j+1]);

      getExtrema<T>(sample_scales, i, keypoints);
    }
}

template void getScaleSpaceExtrema<int>(const vector< vector< Mat > >&,
    vector< KeyPoint >&);

template<typename T>
void getExtrema(const vector< Mat >& sample_scales, const int octaves,
    vector< KeyPoint >& keypoints) {
  for(int i = 0; i < sample_scales[0].rows; i++)
    for(int j = 0; j < sample_scales[0].cols; j++) {
      int pixel = sample_scales[0].at<T>(i, j);

      Rect r(i-1, j-1, 3, 3);
      double max, min;
      for(int s = 0; s < (int)sample_scales.size(); s++) {
        Mat sample = sample_scales[s](r);
        double tempmin, tempmax;
        cv::minMaxLoc(sample, &tempmin, &tempmax);
        max = tempmax > max ? tempmax : max;
        min = tempmin > min ? tempmin : min;
      }

      if (pixel == max || pixel == min)
        keypoints.push_back(KeyPoint(i, j, -1.0f, pixel, octaves, -1));
    }
}

template void getExtrema<int>(const vector< Mat >&, const int, vector< KeyPoint >& );
