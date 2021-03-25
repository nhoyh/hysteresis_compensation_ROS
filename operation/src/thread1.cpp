#include <ros/ros.h>
#include <ros/network.h>
#include <string>
#include <std_msgs/String.h>
#include <sstream>
#include <math.h>

#include "../include/nobaek/mainwindow.h"
#include "../include/nobaek/thread1.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"

//for dataconstruction aurora feedback  lbpe 1 0.01
#define gain_P 3
#define gain_I 0.0
#define gain_D 0.0
#define gain_A 1/gain_P
#define output_limit 120


namespace icra {


Thread1::Thread1()
{
    bRun = false;
    nInterval = 0;

    compensator_on = false;
    vision_pose_estimation = 0.0;
    fusion_pose_estimation = 0.0;

    for(int i=0; i<2;i++)
    {
        precompensated_value[i] =0.0;
        compensation_desired[i] = 0.0;

        current_error[i] = 0.0;
        current_error_dot[i] = 0.0;
        error_sum[i] = 0.0;
        pre_error[i] = 0.0;
        feedback_value[i] = 0.0;
    }

    for(int i =0;i<3;i++) gain_[i] = 0.0;

    for(int i =0;i<2;i++) output_value[i] = 0.0;
    for(int i =0;i<2;i++) pre_output_value[i] = 0.0;

}



void Thread1::run()
{

    m_tm.start();

    // thread control
    bool bIdle = false;
    qint64 nStart = 0.0;
    qint64 nEnd = nStart;

    while (!bRun)
    {
        nEnd = m_tm.nsecsElapsed();
        if (nEnd-nStart < del_time)    bIdle = true;    // measuring 1 (ms) loop
        else                           bIdle = false;

        if(!bIdle)
        {
           if(pWnd->bThread_flag)
           {
                if(compensator_on==true) pThread2->Compensated_angle[0] = feedback_compensator();

           }
           nInterval = (nEnd-nStart)/del_time;  // control loop interval (ms)
           nStart = nEnd;
        }
        else usleep(1000);
    }
    bRun = false;
}

void Thread1::stop()
{
    bRun = true;
}

double Thread1::feedback_compensator()
{

    // std::round(pThread->pitch_angle_result*10)/10
    current_error[0] = pThread2->desired_posi[0] - pEstimator->ave_angle; //pEstimator->aurora_pitch;

    error_sum[0] += current_error[0]*dt;

    //Anti-wind up
    if(abs(pre_output_value[0]>output_limit)) error_sum[0] -= (1/gain_[0])*(pre_output_value[0] - output_value[0]);

    current_error_dot[0] = (current_error[0] - pre_error[0])/dt;
    output_value[0] = gain_[0] * current_error[0] + gain_[2] * error_sum[0] + gain_[1] * current_error_dot[0];

    //output saturation
    if(output_value[0] > output_limit) output_value[0] = output_limit;
    else if(output_value[0] < -output_limit) output_value[0] = -output_limit;

    pre_error[0] = current_error[0];
    pre_output_value[0] = output_value[0];

//    ROS_INFO("compensation =%f %.10f %.10f %f %f %f",pThread2->desired_posi[0],current_error[0],pre_error[0],error_sum[0],current_error_dot[0], output_value[0]);

    return output_value[0];
}



}  // namespace da_robot
