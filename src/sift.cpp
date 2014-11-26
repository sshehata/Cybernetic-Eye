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
 *         Author:  Samy Shihata (sshihata), Mohamed Ashraf (m0hamed), Hossam Ahmed (hoss93)
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
using cv::filter2D;
using cv::KeyPoint;
using cv::Point;
using cv::BORDER_DEFAULT;
using cv::Mat_;
using std::cout;
using std::endl;
using cv::Size;
using cv::Ptr;

template<typename T>
void getScaleSpaceExtrema(const vector< vector< Mat > >& pyr,
    vector< KeyPoint >& keypoints) {
  int octaves = (int) pyr.size();
  int scales = (int) pyr[0].size() - 1;
  for (int i = 0; i < octaves; i++)
    for (int j = 1; j < scales; j++) {
      vector< Mat > sample_scales;
      //sample_scales.push_back(pyr[i][j]);
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
        keypoints.push_back(KeyPoint(j, i, 0,-1.0f, pixel, octave));
      }
    }
  }
}

template void getExtrema<int>(const vector< Mat >&, const int, vector< KeyPoint >& );
template void getExtrema<uchar>(const vector< Mat >&, const int, vector< KeyPoint >& );
template void getExtrema<double>(const vector< Mat >&, const int, vector< KeyPoint >& );

template<typename T>
void findSiftInterestPoint(const Mat& input, vector<KeyPoint>& keypoints,
    int type, bool normalize, bool visualize) {
  Mat image(input.rows, input.cols, type);
  if(input.type() != type) {
    input.convertTo(image, image.type());
  } else {
    image = input;
  }
  if (normalize) {
    cv::normalize(image, image, 0, 1, cv::NORM_MINMAX);
  }

  cv::Ptr<cv::FilterEngine> g = cv::createGaussianFilter(image.type(),
      cv::Size(3, 3), sqrt(2));
  Mat bigger_image = upSample<T>(image);
  Mat bigger_sharpened(bigger_image.rows, bigger_image.cols,
      bigger_image.type());
  g->apply(bigger_image, bigger_sharpened);

  vector<vector<Mat>> pyramid;
  buildGaussianPyramid<T>(bigger_sharpened, pyramid,
      SIFT_NUMBER_OF_OCTAVES);
  if (visualize) {
    for(int i=0; i<pyramid.size(); i++) {
      for(int j=0; j<pyramid[0].size(); j++) {
        char name[2];
        std::sprintf(name, "%i", i*3 + 1);
        imshow(name, pyramid[i][j]);
        //cv::waitKey(0);
      }
    }
  }

  vector<vector<Mat>> dog_pyramid = buildDogPyramid(pyramid);
  if (visualize) {
    for(int i=0; i < dog_pyramid.size(); i++) {
      for(int j=0; j < dog_pyramid[0].size(); j++) {
        char name[2];
        std::sprintf(name, "%i", i*3 + 1);
        imshow(name, dog_pyramid[i][j]);
        //cv::waitKey(0);
      }
    }
  }

  getScaleSpaceExtrema<T>(dog_pyramid, keypoints);
  if (visualize)
    cout << "Keypoints found: " << keypoints.size() << endl;

  keypoints = cleanPoints(bigger_sharpened, keypoints);
  if (visualize)
    cout << "Valid Keypoints found: " << keypoints.size() << endl;
  if (visualize) {
    Mat im2(image.rows, image.cols, CV_32FC1);
    image.convertTo(im2, im2.type());
    cv::normalize(im2, im2, 0, 1, cv::NORM_MINMAX);
    Mat color_image;
    cvtColor(im2, color_image, CV_GRAY2RGB);
    for (int i = 0; i < keypoints.size(); ++i) {
      KeyPoint point = keypoints[i];
      int octave = point.octave;
      double factor = pow(2,octave-1);
      int row_index = (int)point.pt.y * factor;
      int col_index = (int)point.pt.x * factor;
      color_image.at<cv::Vec3f>(row_index,col_index) = {0, 0, 1};
    }
    cv::namedWindow("keypoints", cv::WINDOW_AUTOSIZE);// Create a window for display.
    imshow("keypoints", color_image.clone());
    cv::waitKey(0);
  }
}

template void findSiftInterestPoint<int>(const Mat&, vector<KeyPoint>&, int,
    bool, bool);
template void findSiftInterestPoint<float>(const Mat&, vector<KeyPoint>&, int,
    bool, bool);
template void findSiftInterestPoint<double>(const Mat&, vector<KeyPoint>&, int,
    bool, bool);
template void findSiftInterestPoint<uchar>(const Mat&, vector<KeyPoint>&, int,
    bool, bool);
template void findSiftInterestPoint<short>(const Mat&, vector<KeyPoint>&, int,
    bool, bool);

vector< KeyPoint > cleanPoints(const Mat& image,const vector< KeyPoint >& keypoints) {
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
  filter2D( image, Dxx, CV_64F , xx);
  filter2D( image, Dxy, CV_64F , xy);
  filter2D( image, Dyy, CV_64F , yy);

  //  The matrices used for operations to check whether a point is clean or not
  Mat trace = Dxx + Dyy;
  // This has to be element wise because it would have been Dxx * Dyy'
  Mat det = Dxx.mul(Dyy) - Dxy.mul(Dxy);
  for (int i = 0; i < keypoints.size(); ++i) {
    KeyPoint point = keypoints[i];
    int octave = point.octave;
    int factor = pow(2,octave);
    int row_index = (int)point.pt.y * factor;
    int col_index = (int)point.pt.x * factor;
    double principal_curvature = det.at<float>(row_index, col_index) -
          ALPHA * pow(trace.at<float>(row_index, col_index),2);
    double pixel_value = fabs(point.response);
    if(fabs(principal_curvature) < PRINCIPAL_CURVATURE_THRESHOLD && pixel_value > RESPONSE_THRESHOLD) {
      valid_keypoints.push_back(keypoints.at(i));
    }
  }
  return valid_keypoints;
}

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
