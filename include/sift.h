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
 *         Author:  Samy Shihata (sshihata), Hossam Ahmed (hoss93)
 *   Organization:  GUC
 *
 * =====================================================================================
 */

#ifndef SIFT_H
#define SIFT_H

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <cmath>
#include "utils.h"

template<typename T>
extern void getScaleSpaceExtrema(const std::vector< std::vector< cv::Mat > >&,
    std::vector< cv::KeyPoint>&);

template<typename T>
extern void getExtrema(const std::vector< cv::Mat >&, const int, std::vector< cv::KeyPoint >&);

void cleanPoints(cv::Mat&, std::vector< cv::KeyPoint>&, int curv_thr);

#endif