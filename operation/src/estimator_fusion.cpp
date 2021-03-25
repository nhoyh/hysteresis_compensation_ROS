#include <ros/ros.h>
#include <ros/network.h>
#include <string>
#include <std_msgs/String.h>
#include <sstream>

#include "../include/nobaek/mainwindow.h"
#include "../include/nobaek/estimator_fusion.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"


namespace icra {

ros::Subscriber pose_estimation_scnn_receiver;
ros::Subscriber calibration_receiver;
ros::Subscriber aurora_receiver;

Estimator::Estimator(){

    estimated_angle = 0.0;
    ave_angle = 0.0;
    for(int i=0;i<7;i++) boucwen_param[i] = 0.0;
    optimization_callback_on = false;
    aurora_callback_start = false;
}



void Estimator::scnn_pose_estimation_callback(const da_msg::vision_estimation::ConstPtr& msg)
{
    estimated_angle = msg->parameter_value[0];
    ave_angle = msg->parameter_value[1];
    qDebug() <<"estimated angle = " << estimated_angle;
}

void Estimator::optimization_callback(const da_msg::command::ConstPtr& msg)
{
    for(int i=0;i<7;i++) boucwen_param[i] = msg->parameter_value[i];
    optimization_callback_on = true;
    qDebug() << "parameter=" << boucwen_param[0] << boucwen_param[1] << boucwen_param[2]
             << boucwen_param[3] << boucwen_param[4] << boucwen_param[5] << boucwen_param[6];
}

void Estimator::aurora_callback(const da_msg::command::ConstPtr& msg)
{
    aurora_pitch = msg->parameter_value[0];
    //qDebug() << "aurora pitch=" << aurora_pitch;
}

void Estimator::run()
{
    ros::start();
    ros::NodeHandle n;

    pose_estimation_scnn_receiver = n.subscribe<da_msg::vision_estimation>("scnn_out", 1, &Estimator::scnn_pose_estimation_callback, this);
    calibration_receiver = n.subscribe<da_msg::command>("optimization_topic",1,&Estimator::optimization_callback,this);
    aurora_receiver = n.subscribe<da_msg::command>("aurora_pub_pose",1,&Estimator::aurora_callback,this);

    ROS_INFO("ik feedback start!!\n");
    start();
    ros::spin();

}



}  // namespace da_robot
