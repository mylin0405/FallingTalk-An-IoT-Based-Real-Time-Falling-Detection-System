#pragma once
/*
Motion History Image Header File
2021.1.9
*/
#include <opencv2/opencv.hpp>
#include <opencv2/optflow/motempl.hpp>

using namespace cv;
using namespace std;
using namespace motempl;

// various tracking parameters (in seconds)
const double MHI_DURATION = 2;
const double MAX_TIME_DELTA = 0.5;
const double MIN_TIME_DELTA = 0.05;
// number of cyclic frame buffer used for motion detection

// ring image buffer
vector<Mat> buf;
int last = 0;

// temporary images
Mat mhi, orient, mask, segmask, zplane;
vector<Rect> regions;

void update_mhi(const Mat& img, Mat& dst, int diff_threshold)
{
    double timestamp = (double)clock() / CLOCKS_PER_SEC; // get current time in seconds
    Size size = img.size();
    int i, idx1 = last;
    Rect comp_rect;
    double count;
    double angle;
    Point center;
    double magnitude;
    Scalar color;

    // allocate images at the beginning or
    // reallocate them if the frame size is changed
    if (mhi.size() != size)
    {
        mhi = Mat::zeros(size, CV_32F);
        zplane = Mat::zeros(size, CV_8U);

        buf[0] = Mat::zeros(size, CV_8U);
        buf[1] = Mat::zeros(size, CV_8U);
    }

    cvtColor(img, buf[last], COLOR_BGR2GRAY); // convert frame to grayscale

    int idx2 = (last + 1) % 2; // index of (last - (N-1))th frame
    last = idx2;

    Mat silh = buf[idx2];
    absdiff(buf[idx1], buf[idx2], silh); // get difference between frames

    threshold(silh, silh, diff_threshold, 1, THRESH_BINARY); // and threshold it
    updateMotionHistory(silh, mhi, timestamp, MHI_DURATION); // update MHI

    // convert MHI to green 8u image
    mhi.convertTo(mask, CV_8U, 255. / MHI_DURATION, (MHI_DURATION - timestamp) * 255. / MHI_DURATION);

    Mat planes[] = { zplane, mask ,zplane };
    merge(planes, 3, dst);

    /*
    // calculate motion gradient orientation and valid orientation mask
    calcMotionGradient(mhi, mask, orient, MAX_TIME_DELTA, MIN_TIME_DELTA, 3);

    // segment motion: get sequence of motion components
    // segmask is marked motion components map. It is not used further
    regions.clear();
    segmentMotion(mhi, segmask, regions, timestamp, MAX_TIME_DELTA);

    // iterate through the motion components,
    // One more iteration (i == -1) corresponds to the whole image (global motion)
    for (i = -1; i < (int)regions.size(); i++) {

        if (i < 0) { // case of the whole image
            comp_rect = Rect(0, 0, size.width, size.height);
            color = Scalar(255, 255, 255);
            magnitude = 100;
        }
        else { // i-th motion component
            comp_rect = regions[i];
            if (comp_rect.width + comp_rect.height < 100) // reject very small components
                continue;
            color = Scalar(0, 0, 255);
            magnitude = 30;
        }

        // select component ROI
        Mat silh_roi = silh(comp_rect);
        Mat mhi_roi = mhi(comp_rect);
        Mat orient_roi = orient(comp_rect);
        Mat mask_roi = mask(comp_rect);

        // calculate orientation
        angle = calcGlobalOrientation(orient_roi, mask_roi, mhi_roi, timestamp, MHI_DURATION);
        angle = 360.0 - angle;  // adjust for images with top-left origin

        count = norm(silh_roi, NORM_L1);; // calculate number of points within silhouette ROI

        // check for the case of little motion
        if (count < comp_rect.width * comp_rect.height * 0.05)
            continue;

        // draw a clock with arrow indicating the direction
        center = Point((comp_rect.x + comp_rect.width / 2),
            (comp_rect.y + comp_rect.height / 2));

        circle(img, center, cvRound(magnitude * 1.2), color, 3, 16, 0);
        line(img, center, Point(cvRound(center.x + magnitude * cos(angle * CV_PI / 180)),
        cvRound(center.y - magnitude * sin(angle * CV_PI / 180))), color, 3, 16, 0);
    }
    */
}