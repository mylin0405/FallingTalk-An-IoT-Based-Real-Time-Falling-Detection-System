/*
main program(.cpp) for falling detections
2021.1.9
*/

#include <iostream>
#include "FallDownDetector.h"
#include "MHI.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    // intitial some parameters
    string file_place = "C:/Users/User/Desktop/Falling_Detection/rgb/";
    int count = 1;
    buf.resize(2);
    Mat image, motion, fgMask;
    Ptr<BackgroundSubtractorMOG2> mog2 = createBackgroundSubtractorMOG2();

    // start detection
    for (;;count++)
    {
        // source file ...
        if (count < 10)
        {
            image = imread(file_place + "rgb_000" + to_string(count) + ".png");
        }
        else if(count >= 10 && count < 100)
        {
            image = imread(file_place + "rgb_00" + to_string(count) + ".png");
        }
        else
        {
            image = imread(file_place + "rgb_0" + to_string(count) + ".png");
        }
        if (image.empty())
            break;

        // main part
        update_mhi(image, motion, 30);
        falldowndetector_img(mog2, image, fgMask, motion, count);

        // show the results
        imshow("Image", image);
        imshow("Motion", motion);
        imshow("fgMask", fgMask);
    }
    return 0;
}