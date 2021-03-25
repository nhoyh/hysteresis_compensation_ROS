#include "../include/opencv/dh_cam.h"

//command topic
using namespace std;
using namespace cv;
using namespace BDH_CAM;

CvSize size;

bool init = false;
cv_bridge::CvImagePtr cv_ptr, cv_ptr_gray;

cv::Mat resize_img;
cv::Mat Img_roi, Input_img;


ImageConverter::ImageConverter()
    : it_(nh_)
{
    image_pub_ = it_.advertise("gan_segmentation", 1); // scnn gan_segmentation
}

ImageConverter::~ImageConverter()
{
}


Mat ImageConverter::img_ROI(cv::Mat& cv_ptr)
{
    Img_roi= cv_ptr(cv::Rect(cv_ptr.cols*5/50.5,cv_ptr.rows*5/28, cv_ptr.cols/2*24/26, cv_ptr.rows*23/28));
    return Img_roi;
//    imshow("roi", Img_roi);
//    waitKey(3);
}
//x:5/50.5
//y:23/28

//**************************algoritm****************************//
void ImageConverter::image_pub_to_gan_segmentation(cv::Mat& cv_ptr)
{
    Input_img = img_ROI(cv_ptr);
    resize(Input_img, Input_img, cv::Size(128,128), 0, 0, CV_INTER_NN);

    ROS_INFO("PUB IMAGE");
    
    sensor_msgs::ImagePtr scnn_msg = cv_bridge::CvImage(std_msgs::Header(),"bgr8",Input_img).toImageMsg(); //mono8
    image_pub_.publish(scnn_msg);
    //imshow("1st",cv_ptr);
    //waitKey(10);

}


