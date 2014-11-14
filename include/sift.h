/*
 * =====================================================================================
 *
 *       Filename:  sift.h
 *
 *    Description:  functions specific to sift algorithm implementation
 *
 *        Version:  1.0
 *        Created:  11/14/2014 08:29:14 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Samy Shihata (sshihata), 
 *   Organization:  GUC
 *
 * =====================================================================================
 */

#ifndef SIFT_H
#define SIFT_H

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <cmath>

using std::vector;
using cv::Mat;
using cv::KeyPoint;
using cv::Rect;

template<typename T>
extern void getScaleSpaceExtrema(const vector< vector< Mat > >&,
    vector< KeyPoint>&);

template<typename T>
extern void getExtrema(const vector< Mat >&, const int, vector< KeyPoint >&);

#endif
