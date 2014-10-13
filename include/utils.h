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

template <typename T>
void Conv (const cv::Mat& kernel, const cv::Mat& signal, 
        const uchar edge_handling, cv::Mat& output) {
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

    while (i < signal.rows - limiti) {
        int j = startj;
        while (j < signal.cols - limitj) {
            output.at<T>(i, j) = kernel.dot(signal(cv::Rect(j - half_cols, i - half_rows, 
                            kernel.rows, kernel.cols)));
            j++;
        }
        i++;
    }
}

#endif
