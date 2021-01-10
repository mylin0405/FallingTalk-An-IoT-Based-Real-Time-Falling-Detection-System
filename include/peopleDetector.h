/*
Simple Simple People Detector use SVM+HOG features
2021.1.9
*/

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void peopleDetector()
{
	Mat image = imread("test.png");

	HOGDescriptor hog;

	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

	vector<cv::Rect> regions;
	hog.detectMultiScale(image, regions, 0, Size(8, 8), Size(32, 32), 1.05, 1);

	for (size_t i = 0; i < regions.size(); i++)
	{
		rectangle(image, regions[i], Scalar(0, 255, 0), 2);
	}

	imshow("test", image);
	waitKey();

}