#pragma once
/*
Fall down detector Header File
2021.1.9
*/
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/tracking.hpp>
#include <cmath>
#include <ctime>
#include <queue>

using namespace cv;
using namespace std;

// store image history information
vector<Point> center_point; // draw center point..(probably 5 points?) 
vector<Point> previous_frame; // store previous contour for iou tracking
double ellip_aspectratio; // contour ellip ratio
int fall_count = 0; // fall count
int ellip_angle; // maybe useful

// Find the biggest contour in the contours set
int FindBiggestContour(vector<vector<Point>> pts)
{
    double area = 0;
    int index = 0;
    for (int i = 0; i < pts.size(); i++)
    {
        if (contourArea(pts[i]))
        {
            area = contourArea(pts[i]);
            index = i;
        }
    }
    return index;
}

void draw_center(Mat &frame)
{
    for (auto i = 0; i < center_point.size(); i++)
    {
        circle(frame, center_point[i], 2, Scalar(49, 214, 60), 2, 8);
    }

    for (auto i = 0; i < center_point.size() - 1; i++)
    {
        center_point[i] = center_point[i + 1];
    }

    center_point.pop_back();
}

int check_fall_count(int fall_count)
{
    if (fall_count > 10)
    {
        return 3;
    }
    else if(fall_count <= 10 && fall_count >0)
    {
        return 2;
    }
    else
    {
        return 1;
    }
}


bool IOU_check(vector<Point> pts)
{
    if (pts.size() < 5 || previous_frame.size() < 5)
    {
        return false;
    }
    RotatedRect ellip_old = fitEllipse(previous_frame);
    RotatedRect ellip_new = fitEllipse(pts);
    vector<Point> inter;
    int res = rotatedRectangleIntersection(ellip_old, ellip_new, inter);
    if (inter.empty() || res == INTERSECT_NONE)
        return 0.0f;
    if (res == INTERSECT_FULL)
        return 1.0f;
    float interArea = contourArea(inter);
    if (interArea / (ellip_old.size.area() + ellip_new.size.area() - interArea) > 0.2)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// BackgroundSubstractor which reads img(file path) from user input
void falldowndetector_img(Ptr<BackgroundSubtractorMOG2> mog2, Mat& frame, Mat& fgMask, Mat& motion, int count)
{
    cout << "image" << count << endl;
    //initial some parameters
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    string file_place = "C:/Users/User/Desktop/Falling_Detection/rgb/";

    //update the background model mog2
    mog2->apply(frame, fgMask);

    //some preprocessing steps
    GaussianBlur(fgMask, fgMask, Size(3, 3), 0, 0);
    threshold(fgMask, fgMask, 235, 255, THRESH_BINARY + CV_THRESH_OTSU);
    dilate(fgMask, fgMask, Mat());
    findContours(fgMask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    // find the biggest Contour in the image
    int bg_index = FindBiggestContour(contours);


    // fit ellipse for the biggest contour
    if (int(contours[bg_index].size()) > 5 && contourArea(contours[bg_index]) > 50)
    {
        if (IOU_check(contours[bg_index]))
        {
            RotatedRect ellip = fitEllipse(contours[bg_index]);
            //cout << ellip.angle << endl;
            //cout << ellip.center << endl;
            //cout << ellip.size.height << endl;
            //cout << ellip.size.width << endl;
            //cout << ellip.size.height / ellip.size.width << endl;
            ellip_aspectratio = ellip.size.height / ellip.size.width;
            ellip_angle = (ellip.angle > 90) ? 180 - ellip.angle : ellip.angle;

            if (center_point.size() <= 5)
            {
                center_point.push_back(ellip.center);
            }
            else
            {
                draw_center(frame);
            }

            if (ellip.size.height > ellip.size.width && ellip_angle < 30)
            {
                if ((fall_count - 1) >= 0)
                {
                    fall_count -= 1;
                }              
            }
            else
            {
                fall_count++;
            }

            switch (check_fall_count(fall_count))
            {
            case 1:
                ellipse(frame, ellip, Scalar(49, 214, 60), 1, 8);
                circle(frame, ellip.center, 2, Scalar(49, 214, 60), 2, 8);
                break;
            case 2:
                ellipse(frame, ellip, Scalar(19, 120, 242), 1, 8);
                circle(frame, ellip.center, 2, Scalar(19, 120, 242), 2, 8);
                break;
            case 3:
                ellipse(frame, ellip, Scalar(0, 0, 255), 1, 8);
                circle(frame, ellip.center, 2, Scalar(0, 0, 255), 2, 8);
                break;
            default:
                ellipse(frame, ellip, Scalar(49, 214, 60), 1, 8);
                circle(frame, ellip.center, 2, Scalar(49, 214, 60), 2, 8);
                break;
            }
        }
        previous_frame.assign(contours[bg_index].begin(), contours[bg_index].end());
    }

    // count motion coefficient from MHI.h
    int motion_count = 0;
    for (int i = 0; i < contours[bg_index].size(); i++)
    {
        //cout << motion.at<Vec3b>(contours[bg_index][i]) << endl;
        if (motion.at<Vec3b>(contours[bg_index][i])[1] != 0)
        {
            motion_count++;
        }

    }
    //cout << "image" << count << ":" << motion_count / contours[bg_index].size() << endl;
    cout << fall_count << endl;
    // write infromation on the source image
    cv::rectangle(frame, Point(10, 2), Point(100, 20), Scalar(255, 255, 255), -1);
    cv::putText(frame, to_string(count), cv::Point(15, 15), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0)); 
    contours.clear();
    hierarchy.clear();
    cv::waitKey(10);
}