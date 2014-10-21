/*
 * =====================================================================================
 *
 *       Filename:  DisplayImage.cpp
 *
 *    Description:  Show an image. Test project for OpenCV
 *
 *        Version:  1.0
 *        Created:  10/04/2014 05:58:57 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Samy Shihata (sshihata), Mohamed Ashraf (m0hamed)
 *   Organization:  GUC
 *
 * =====================================================================================
 */

#include <cstdio>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char** argv) {

	Mat image;
	if (argc != 2) {
		printf("usage: DisplayImage <ImagePath>\n>");
		return -1;
	}

	image = imread(argv[1], 1);

	if (!image.data) {
		printf("no image data\n");
		return -1;
	}

	namedWindow("Display Image", CV_WINDOW_AUTOSIZE);
	imshow("Display Image", image);

	waitKey(0);
	return 0;
}
