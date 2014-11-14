/*
 * =====================================================================================
 *
 *       Filename:  utils.h
 *
 *    Description:  A collection of math utility functions 
 *
 *        Version:  1.0
 *        Created:  10/13/2014 11:37:05 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Samy Shihata (sshihata), 
 *   Organization:  GUC
 *
 * =====================================================================================
 */

#ifndef UTILS_H
#define UTILS_H

#include <opencv2/core/core.hpp>
#include <iostream>
#define CONV_IGNORE_EDGE 0

template<typename T>
extern void conv  (const cv::Mat& kernel, const cv::Mat& signal,
        const uchar edge_handling, cv::Mat& output);

#endif
