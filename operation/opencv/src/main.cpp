#include <string>
#include <iostream>
#include "opencv2/videoio.hpp"
#include "opencv2/core/mat.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "../include/opencv/dh_cam.h"

using namespace cv;
using namespace std;
using namespace BDH_CAM;

double end, begin;

int main(int argc, char** argv)
{
        Mat frame;
        VideoCapture cap(0);

        ros::init(argc, argv, "OPENCV");

        ImageConverter img_converter;


        if (!cap.isOpened())
        {
            ROS_INFO("Cannot open the video cam") ;
            return -1;
        }

//        cap.set(CV_CAP_PROP_SETTINGS,0.0);
        cap.set(CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));
        cap.set(CV_CAP_PROP_FPS, 30);
        cap.set(CV_CAP_PROP_FRAME_WIDTH, 1920); // input size???
        cap.set(CV_CAP_PROP_FRAME_HEIGHT, 1080); // visually size.. no input size
        cap.set(CV_CAP_PROP_BRIGHTNESS, 0.5); // 0 to 1
//        cap.set(CV_CAP_PROP_CONTRAST, 11);
//        cap.set(CV_CAP_PROP_SATURATION, 1);
//        cap.set(CV_CAP_PROP_GAIN, 0);
//        cap.set(CV_CAP_PROP_HUE,0);

//        float Brightness = cap.get(CV_CAP_PROP_BRIGHTNESS);
//        float Contrast   = cap.get(CV_CAP_PROP_CONTRAST );
//        float Saturation = cap.get(CV_CAP_PROP_SATURATION);
//        float Gain       = cap.get(CV_CAP_PROP_GAIN);




        while ( ros::ok() )
        {
//            cap.read(frame);
            cap >> frame;
            if (frame.empty())
            {
                ROS_INFO("NO capture");
                break;
            }
//            ROS_INFO("1");
            img_converter.image_pub_to_gan_segmentation(frame);
//            imshow("Color", frame);

//            ROS_INFO("2");
//            waitKey(10);
//            if (waitKey(10) == 27)
//                ROS_INFO("9");
//                break;

        }
        cap.release();
        return 0;

}

