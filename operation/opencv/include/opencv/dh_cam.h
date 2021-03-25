#ifndef BDH_camlib_H
#define BDH_camlib_H

#include <string>
#include <vector>

#include <ros/ros.h>
#include <ros/node_handle.h>
#include <image_transport/image_transport.h>
#include <image_transport/subscriber.h>
#include <image_transport/transport_hints.h>
#include <nodelet/nodelet.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>




#include <opencv2/bgsegm.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<opencv2/objdetect/objdetect.hpp>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include "std_msgs/Float64.h"
#include "geometry_msgs/Pose.h"

#include <ros/console.h>
#include <ros/node_handle.h>
#include <ros/publisher.h>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <time.h>
#include <sensor_msgs/Image.h>
//#include <opencv/scnn_input.h>

//define
#define Width 1920
#define Height 1080
#define Threshold 127
#define camera_num 1

#define INIT_TIME 100

using namespace std;
using namespace cv;


namespace BDH_CAM{


    class ImageConverter
    {
        protected:
            ros::NodeHandle nh_;
            image_transport::ImageTransport it_;
            image_transport::Subscriber image_sub_;
            image_transport::Publisher image_pub_;

            clock_t begin, end;
        public:
            ImageConverter();
            ~ImageConverter();


            //watershed
            void image_pub_to_gan_segmentation(cv::Mat& cv_ptr);

            Mat img_ROI(cv::Mat& cv_ptr);


        private:


    };


}



#endif
