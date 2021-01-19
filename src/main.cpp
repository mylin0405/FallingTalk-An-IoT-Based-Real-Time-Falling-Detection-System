/*
main program(.cpp) for falling detections
2021.1.19 ver.
*/

#include <iostream>
#include <zmq.h>
#include "FallDownDetector.h"
#include "MHI.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    // intitial some parameters & connect to server
    string file_place = "C:/Users/User/Desktop/Falling_Detection/rgb722/";
    Mat image, motion, fgMask;
    Ptr<BackgroundSubtractorMOG2> mog2 = createBackgroundSubtractorMOG2();
    int count = 1; // count frame
    buf.resize(2); // buffer for MHI
    void* context = zmq_ctx_new();
    void* requester = zmq_socket(context, ZMQ_REQ);
    zmq_connect(requester, "tcp://localhost:5555");
    char buffer[10]; // buffer for zmq
    VideoCapture cap(0);

    // start detection
    for (;;count++)
    {
        /*
        if (!cap.isOpened())
        {
            cout << "open camera failed" << endl;
            break;
        }
        cap >> image;
        */
        
        /*
        // source file from falldetection dataset
        if (count < 10)
        {
            image = imread(file_place + "rgb_000" + to_string(count) + ".png");
        }
        else if(count >= 10 && count < 100)
        {
            image = imread(file_place + "rgb_00" + to_string(count) + ".png");
        }
        else if(count < 1000)
        {
            image = imread(file_place + "rgb_0" + to_string(count) + ".png");
        }
        else
        {
            image = imread(file_place + "rgb_" + to_string(count) + ".png");
        }        
        if (image.empty())
            break;
        */

        // main solution part
        update_mhi(image, motion, 30);
        falldowndetector_img(mog2, image, fgMask, motion, count);
        string temp = std::to_string(check_fall_count(fall_count));
        cout << temp << endl;
        //zmq_send(requester, temp.c_str(), 1, 0);
        //zmq_recv(requester, buffer, 10, 0);

        // show the results
        imwrite("C:/Users/User/Desktop/Falling_Detection/test/" + to_string(count) + ".jpg", image);
        imshow("Image", image);
        imshow("Motion", motion);
        imshow("fgMask", fgMask);
        waitKey(100);
    }

    zmq_close(requester);
    zmq_ctx_destroy(context);
    return 0;
}