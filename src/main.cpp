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
 *         Author:  Samy Shihata (sshihata),
 *   Organization:  GUC
 *
 * =====================================================================================
 */

#include <opencv2/core/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include "utils.h"

using namespace std;

int main () {
    cv::Mat signal(5, 5, CV_8UC1);
    cv::randu(signal, cv::Scalar(0), cv::Scalar(255));
    cout << "signal Matrix = \n" << signal << endl;

    cv::Mat kernel = (cv::Mat_<unsigned char>(3,3) << 1, 2, 1, 2, 4, 2, 1, 2, 1);
    cout << "kernel Matrix = \n" << kernel << endl;

    cv::Mat output(signal.rows, signal.cols, CV_32SC1);
    Conv<int>(kernel, signal, CONV_IGNORE_EDGE, output);

    cout << "Conv Matrix: \n" << output << endl;
    return 0;
}
