/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  main src file, contains entry point
 *
 *        Version:  1.0
 *        Created:  10/13/2014 02:24:40 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Samy Shihata (sshihata), Mohamed Ashraf (m0hamed), Hossam Ahmed (hoss93)
 *   Organization:  GUC
 *
 * =====================================================================================
 */

#include <opencv2/core/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "utils.h"
#include "sift.h"
#include <cmath>

using std::cout;
using std::endl;
using std::vector;
using cv::Mat;
using cv::KeyPoint;
using std::vector;
using cv::imread;

int main (int argc, char**argv) {

  if(argc != 2)
  {
    cout << " Usage: cybervis ImageToLoadAndDisplay" << endl;
    return -1;
  }

  Mat image;
  image = imread(argv[1], 0);

  if(!image.data)
  {
    cout << "Could not open or find the image" << endl ;
    return -1;
  }

  cv::namedWindow("Cybervis", cv::WINDOW_AUTOSIZE);// Create a window for display.
  imshow("Cybervis", image);

  Mat image_double(image.rows, image.cols, CV_64FC1);
  image.convertTo(image_double, image_double.type());
  cv::normalize(image_double, image_double, 0, 1, cv::NORM_MINMAX);

  cv::waitKey(0);

  Mat smaller = downSample<double>(image_double);

  imshow("Cybervis", smaller);

  cv::waitKey(0);

  vector<vector<Mat>> pyramid;
  buildGaussianPyramid<double>(image_double, pyramid, 3);

  for(int i=0; i<3; i++) {
    for(int j=0; j<5; j++) {
      char name[2];
      std::sprintf(name, "%i", i*3 + 1);
      imshow(name, pyramid[i][j]);
      cv::waitKey(0);
    }
  }

  vector<vector<Mat> > dog_pyramid = buildDogPyramid(pyramid);

  for(int i=0; i < dog_pyramid.size(); i++) {
    for(int j=0; j < dog_pyramid[0].size(); j++) {
      char name[2];
      std::sprintf(name, "%i", i*3 + 1);
      imshow(name, dog_pyramid[i][j]);
      cv::waitKey(0);
    }
  }

  vector< KeyPoint > keypoints;
  getScaleSpaceExtrema<uchar>(dog_pyramid, keypoints);
  cout << keypoints.size() << endl;
  return 0;
}
