#include <ros/ros.h>
#include <ros/network.h>
#include <string>
#include <std_msgs/String.h>
#include <sstream>
#include <math.h>
#include <QDebug>

#include "../include/nobaek/mainwindow.h"
#include "../include/nobaek/thread2.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"



namespace icra {

Thread2::Thread2()
{
    bRun = false;
    nInterval = 0;
    time = 0.0;
    time_ = 0.0;
    theta = 0.0;

    for (int i=0;i<DOF;i++)
    {
        pre_desired_posi[i] = 0.0;
        pre_desired_posi_dot[i] = 0.0;
        desired_posi[i] = 0.0;
    }

    for (int i=0;i<4;i++)
    {
        final_pos_value[i] = 2048;
        init_offset[i] = 2048;
    }

    j = 0.0;
    La_d_1 = 0.0;
    La_d_2 = 0.0;


    //LDG -  continuum 4 bedning joint parameter
    R = 1.5;
    B = 1.2;
    d = 2.2;//1.2, 1.7;

    L_a = 0.0;
    L_b = 0.0;

    //trajectory
    start_sin_traj = false;
    filter_on      = false;
    sin_magnitude = 0.0;
    master_on   = false;

    ik_pose_pub_start = false;

    begin = 0.0;
    end  = 0.0;

    for(int i=0;i<2;i++)
    {
        Compensated_angle[i] = 0.0;
        desired_posi_ddot[i] = 0.0;
        desired_posi_dot[i]  = 0.0;

        predirection_posi[i] = 0.0;
        direction_vel[i] = 0.0;
        predirection_vel[i]  = 0.0;

        backlash_compensation[i] = 0.0;
        kinematic_output[i] = 0.0;
    }
    //tension controller
    pre_tension_onoff = false;

    for (int i=0;i<bending_motor_num;i++)
    {
        pid_t.desired_pre_tension[i] = 0.0;
        pid_t.tension_controller_output[i] = 0.0;
        pid_t.pre_tension_controller_output[i] = 0.0;
        pid_t.tension_err[i] = 0.0;
        pid_t.tension_err_sum[i] = 0.0;
        pid_t.tension_err_diff[i] = 0.0;
        pid_t.past_tension_err[i] = 0.0;
    }
    pid_t.P_[0] = 0.1;
    pid_t.P_[1] = 0.1;
    pid_t.P_[2] = 0.1;
    pid_t.P_[3] = 0.1;

    pid_t.I_[0] = 0.0;
    pid_t.I_[1] = 0.0;
    pid_t.I_[2] = 0.0;
    pid_t.I_[3] = 0.0;

    pid_t.D_[0] = 0;
    pid_t.D_[1] = 0;
    pid_t.D_[2] = 0;
    pid_t.D_[3] = 0;

    for (int i=0;i<bending_motor_num;i++)
        pid_t.A_[i] = 1/pid_t.P_[i];
    pid_t.limit_pid = 50;
    pid_t.dtt = 0.01;

    for (int i=0;i<bending_motor_num;i++)
    {
        current_val[i] = 0.0;
        sensor_val[i] = 0.0;
        loadcell1_ago[i] = 0.0;
        loadcell2_ago[i] = 0.0;
        loadcell3_ago[i] = 0.0;
        loadcell4_ago[i] = 0.0;

        LPF_motor_current[i] = 0.0;
        LPF_alpha[i] = 0.9;

    }

    pitch1_motor_current = 0.0;
    pitch2_motor_current = 0.0;
    yaw1_motor_current = 0.0;
    yaw2_motor_current = 0.0;

    //configuration select 0 ~ 270(10 cases)
    configuration = 0;
    compensation_end = false;
    conf_calibration = false;


    feed_forward_on = false;
    data_save_on= false;
    data_time = 0.0;
    send = true;
    for(int k =0;k<bending_range;k++) bending[k] = k; // 0~359

    traj_stop = false;

    scnn_data_flag = false;
    screenshot_ = false;

    //FF

    FF.des_pos_old = 0.0;
    FF.H = 0.0;
    FF.H_old = 0.0;
    FF.des_vel = 0.0;
    FF.H_dot = 0.0;
    FF.dtt = 0.001;//0.025;

    FF.param[0] = 0.0; //0.376;
    FF.param[1] = 0.0; //0.015;
    FF.param[2] = 0.0; //0.037;
    FF.param[3] = 0.0; //1.103;
    FF.param[4] = 0.0; //0.967;
    FF.param[5] = 0.0; //3.325;

    feed_forward_start = true;
}

double Thread2::MAF_loadcell1(double loadcell1_cur_val)
{
    double loadcell1_avg = 0.0;
    loadcell1_ago[4] = loadcell1_ago[3];
    loadcell1_ago[3] = loadcell1_ago[2];
    loadcell1_ago[2] = loadcell1_ago[1];
    loadcell1_ago[1] = loadcell1_ago[0];
    loadcell1_ago[0] = loadcell1_cur_val;
    loadcell1_avg = (loadcell1_ago[0] + loadcell1_ago[1] + loadcell1_ago[2] + loadcell1_ago[3] + loadcell1_ago[4])/5.0;
    return loadcell1_avg;
}

double Thread2::MAF_loadcell2(double loadcell2_cur_val)
{
    double loadcell2_avg = 0.0;
    loadcell2_ago[4] = loadcell2_ago[3];
    loadcell2_ago[3] = loadcell2_ago[2];
    loadcell2_ago[2] = loadcell2_ago[1];
    loadcell2_ago[1] = loadcell2_ago[0];
    loadcell2_ago[0] = loadcell2_cur_val;
    loadcell2_avg = (loadcell2_ago[0] + loadcell2_ago[1] + loadcell2_ago[2] + loadcell2_ago[3] + loadcell2_ago[4])/5.0;
    return loadcell2_avg;
}

double Thread2::MAF_loadcell3(double loadcell3_cur_val)
{
    double loadcell3_avg = 0.0;
    loadcell3_ago[4] = loadcell3_ago[3];
    loadcell3_ago[3] = loadcell3_ago[2];
    loadcell3_ago[2] = loadcell3_ago[1];
    loadcell3_ago[1] = loadcell3_ago[0];
    loadcell3_ago[0] = loadcell3_cur_val;
    loadcell3_avg = (loadcell3_ago[0] + loadcell3_ago[1] + loadcell3_ago[2] + loadcell3_ago[3] + loadcell3_ago[4])/5.0;
    return loadcell3_avg;
}

double Thread2::MAF_loadcell4(double loadcell4_cur_val)
{
    double loadcell4_avg = 0.0;
    loadcell4_ago[4] = loadcell4_ago[3];
    loadcell4_ago[3] = loadcell4_ago[2];
    loadcell4_ago[2] = loadcell4_ago[1];
    loadcell4_ago[1] = loadcell4_ago[0];
    loadcell4_ago[0] = loadcell4_cur_val;
    loadcell4_avg = (loadcell4_ago[0] + loadcell4_ago[1] + loadcell4_ago[2] + loadcell4_ago[3] + loadcell4_ago[4])/5.0;
    return loadcell4_avg;
}


void Thread2::tension_controller()
{
    for(int id=0; id<bending_motor_num;id++)
        pid_t.tension_err[id] = pid_t.desired_pre_tension[id] - loadcell_tension[id];

    for(int id=0; id<bending_motor_num;id++)
    {
        pid_t.tension_err_sum[id] += pid_t.tension_err[id]*pid_t.dtt;
        if(abs(pid_t.pre_tension_controller_output[id]) > pid_t.limit_pid) pid_t.tension_err_sum[id] -= pid_t.A_[id]*(pid_t.pre_tension_controller_output[id] - pid_t.tension_controller_output[id]);
    }
    for(int id=0; id<bending_motor_num;id++)
        pid_t.tension_err_diff[id] = (pid_t.tension_err[id] - pid_t.past_tension_err[id]) / pid_t.dtt;


    for(int id=0; id<bending_motor_num;id++)
        pid_t.tension_controller_output[id] += pid_t.P_[id]*pid_t.tension_err[id] + pid_t.I_[id]*pid_t.tension_err_sum[id] + pid_t.D_[id]*pid_t.tension_err_diff[id];

    for(int id=0; id<bending_motor_num;id++)
        pid_t.past_tension_err[id] = pid_t.tension_err[id];

    final_pos_value[0] = 2048 + pid_t.tension_controller_output[3]; //YAW_1
    final_pos_value[1] = 2048 + pid_t.tension_controller_output[2]; //PITCH_1
    final_pos_value[2] = 2048 + pid_t.tension_controller_output[1]; //PITCH_2
    final_pos_value[3] = 2048 + pid_t.tension_controller_output[0]; //YAW_2

    for(int id=0; id<bending_motor_num;id++)
        init_offset[id] = final_pos_value[id];

    //qDebug() << final_pos_value[0] << final_pos_value[1] << final_pos_value[2] <<final_pos_value[3];
}


void Thread2::Inverse_kinematics_Model(double command, int dof)
{
    theta = DTR(command); // degree to rad

    L_a = (2*R - 2*d*sin(theta/(2*Num_of_joint)) - 2*(R-B)*cos(theta/(2*Num_of_joint)) - 2*B)*Num_of_joint;
    L_b = (2*R + 2*d*sin(theta/(2*Num_of_joint)) - 2*(R-B)*cos(theta/(2*Num_of_joint)) - 2*B)*Num_of_joint;

    kinematic_output[0] = -L_a * LENGTH_TO_TIC;
    kinematic_output[1] = -L_b * LENGTH_TO_TIC;

    if(dof == BENDING_PITCH)
    {
        final_pos_value[1] = init_offset[1] + kinematic_output[0];
        final_pos_value[2] = init_offset[2] + kinematic_output[1];
    }
    else if(dof==BENDING_YAW)
    {
        final_pos_value[0] = init_offset[0] + kinematic_output[0];
        final_pos_value[3] = init_offset[3] + kinematic_output[1];
    }
}



void Thread2::run()
{
    //ros
    ros::start();
    ros::NodeHandle n;
    optimization_start_pub = n.advertise<da_msg::command>("optimization_start",1);//image_save_trajec
    image_save_pub = n.advertise<da_msg::command>("image_saver",100);//image_save_trajec
    feedback_start_pub = n.advertise<da_msg::command>("des_pos",1);//image_save_trajec

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
               time_ += 0.001;

               // LDG - Loadcell val after MAF(moving average filter)
                loadcell_tension[0] = MAF_loadcell1(pLoadcell->m[0].dData[0]);
                loadcell_tension[1] = MAF_loadcell2(pLoadcell->m[0].dData[1]);
                loadcell_tension[2] = MAF_loadcell3(pLoadcell->m[0].dData[2]);
                loadcell_tension[3] = MAF_loadcell4(pLoadcell->m[0].dData[3]);


               //(1) function for pre tension
               if(pre_tension_onoff == true)
                   tension_controller();

               //(2) start operating main algorithm
               else if(pre_tension_onoff == false)
               {                
                   //if(data_save_on==true)  data_save_trajectory_bending();
                   //input trajectory
                   if(start_sin_traj==true)                        desired_posi[0] =  sinusoidal_trajectory();
                   if(conf_calibration==true)                      desired_posi[0] = traj_calibration_func();
//                   else if(start_sin_traj==false && master_on==true)    tele_op_traj();
//                   if(scnn_data_flag == true)
//                   {
//                       SCNN_data_save_traj();
//                   }
                   for (int i=0;i<2;i++)
                   {
                       desired_posi_dot[i] = (desired_posi[i] - pre_desired_posi[i])/FF.dtt;
                       pre_desired_posi[i] =  desired_posi[i];
                   }


                   //inverse kinematics model
                   if(feed_forward_on == true && pThread1->compensator_on==false && feed_forward_start ==true)
                   {
                        Compensated_angle[0] = feed_forward_compensator(desired_posi[0]);
                        Inverse_kinematics_Model(Compensated_angle[0], BENDING_PITCH);
                   }
                   if(feed_forward_on == false && pThread1->compensator_on==false)
                   {
                        Inverse_kinematics_Model(desired_posi[0], BENDING_PITCH);
                   }
                   //feedback test
                   if(pThread1->compensator_on==true && feed_forward_on == false)
                   {
                       Inverse_kinematics_Model(desired_posi[0]+Compensated_angle[0], BENDING_PITCH);
                   }


               }
           }
           nInterval = (nEnd-nStart)/del_time;  // control loop interval (ms)
           nStart = nEnd;
        }
        else   usleep(1000);
    }
    bRun = false;
}

void Thread2::stop()
{
    bRun = true;
}

void Thread2::pub_image_screen_shot() //p=5, i=0.17
{
    da_msg::command saver_pub_msg;
    saver_pub_msg.parameter_value.push_back(desired_posi[0]); //bending
    saver_pub_msg.start =1;
    image_save_pub.publish(saver_pub_msg);
    saver_pub_msg.parameter_value.clear();

    saver_pub_msg.parameter_value.push_back(desired_posi[0]); //bending
    saver_pub_msg.start =0;
    image_save_pub.publish(saver_pub_msg);
    saver_pub_msg.parameter_value.clear();
}
//case 1
//    double exp_param[6] = { 0.133,   4.4064, -0.6567, 11.7446,  0.703,  25.6058}; // 1
//    double exp_param[6] = { 0.1713,  0.7333, -0.0325, 10.9294,  0.7053, 21.8239};
//    double exp_param[6] = {1.92200e-01,  3.59200e-01, -4.60000e-03,  2.42055e+01,  6.88800e-01,1.89096e+01};
//    double exp_param[6] ={2.12000e-01,  3.15000e-02, -2.56000e-02,  4.18254e+01,  6.93500e-01, 1.71331e+01};
//case 2
//    double exp_param[6] = {0.2766,  0.0936,  0.0181,  6.74,    0.905,  16.9116};
//    double exp_param[6] = {2.82200e-01,  4.00000e-03, -3.30000e-03,  4.05215e+01,  9.05300e-01,  1.65583e+01};
//    double exp_param[6] = {0.1963,  1.4604, -0.0766,  9.8482,  0.8902, 23.1189};
//    double exp_param[6] = {0.2526,  0.169,   0.0365,  5.8751,  0.9055, 18.7491};
//    double exp_param[6] = {0.2254,  0.2936,  0.036,   5.6642,  0.897,  20.4191};
//case 3
//    double exp_param[6] = {0.4081,  0.0309, -0.0258, 13.414,   0.8185, 10.4985};
//    double exp_param[6] = {0.1771, 19.5703,  0.9965,  8.098,   0.8038, 25.2004};
//    double exp_param[6] = {0.4793,  0.0326, -0.0153,  6.8613,  0.8017,  8.7491};
//    double exp_param[6] = {2.255,  0.0298, 0.036,  1.9279, 0.8341, 2.3927};
//    double exp_param[6] = {4.61700e-01,  3.70000e-03, -3.20000e-03,  1.51416e+01,  8.16200e-01,   9.24540e+00};

double Thread2::feed_forward_compensator(double des_pos)
{
    double output_value = 0.0;
//    double exp_param[6] = {0.2223,  2.8662, -0.1143,  5.0014,  1.0896, 26.9956};
//    double exp_param[6] = {0.2089,  3.0128, -0.7119,  5.1264,  1.0758, 25.7102};
//    double exp_param[6] = {0.493,   0.0281, -0.0279, 23.1659,  1.0358, 10.9504};
//    double exp_param[6] = {0.4565,  0.0772, -0.0743, 19.0217,  1.054,  12.1228};
    double exp_param[6] = {0.3925,  0.4624, -0.3181,  8.4292,  1.0722, 14.2741};

    for(int i=0;i<6;i++) FF.param[i] = exp_param[i];

    FF.des_vel = (des_pos - FF.des_pos_old)/FF.dtt;
    FF.H_dot = FF.param[0]*FF.des_vel - FF.param[1]*abs(FF.des_vel)*pow(abs(FF.H),FF.param[3]-1)*FF.H - FF.param[2]*FF.des_vel*pow(abs(FF.H),FF.param[3]);
    FF.H = FF.H_old + FF.H_dot*FF.dtt;

    //old value
    FF.des_pos_old = des_pos;
    FF.H_old = FF.H;

    output_value = (des_pos + FF.param[5]*FF.H)/FF.param[4]; //gamma, beta, alpha

    qDebug() <<"pos, vel, H, Hdot, out = " << des_pos << FF.des_vel << FF.H << FF.H_dot <<  output_value ;
    return output_value;
}



/////////////////////////////////trajectory/////////////////////////////////////

double Thread2::sinusoidal_trajectory()
{
    double sin_input = 0.0;
    sin_input = sin_magnitude/2 - sin_magnitude*(cos(time))/2;
//    sin_input = sin_magnitude*(sin(time));

    time += 0.0005;//0.025;
    if(time > 20)
    {
        start_sin_traj = false;
        time = 0.0;
    }
    return sin_input;
}

double Thread2::traj_calibration_func()
{
    //ros pub to nb_pose_estimation
    da_msg::command feedback_start_msg;
    //feedback_start_msg.start =1;
    feedback_start_msg.parameter_value.push_back(desired_posi[0]);
    feedback_start_pub.publish(feedback_start_msg);
    feedback_start_msg.parameter_value.clear();

    double sin_input = 0.0;
//    sin_input = sin_magnitude/2 - sin_magnitude*(cos(time))/2; //periodic
//    sin_input = sin_magnitude*sin(time);
    sin_input = (1.2*sin_magnitude - 1.2*sin_magnitude*(cos(time)) + sin_magnitude/2
                - sin_magnitude*(cos(0.25*time))/2 + sin_magnitude/2 - sin_magnitude*(cos(0.5*time))/2)*60/225;

    time += 0.001;

    if(time > 25)
    {
        conf_calibration = false;
        time = 0.0;
        pWnd->write_on = false;
        compensation_end = true;
        da_msg::command calibration;
        calibration.start = 1;
        optimization_start_pub.publish(calibration);
    }
    qDebug() << sin_input;
    return sin_input;
}

#define TIC_TO_LENGTH_TRANS          (150.0/2900.0)   // 3900(tic)=200(mm)
#define TIC_TO_ANGLE_YAW             (2*PI/(360.0*4.0*32/14))
#define TIC_TO_ANGLE_YAW2            (2*PI/(360.0*4.0))
#define TIC_TO_ANGLE_PITCH           (360.0/4096.0)
#define TIC_TO_ANGLE_ROLL            (2*PI/(360.0*4.0))

void Thread2::tele_op_traj()
{
    desired_posi[0] =  (double)pEnc->m[0].nPos[0]*TIC_TO_ANGLE_PITCH;  //pitch

    // LDG - desired pos limit : -100 ~ 100
    if(desired_posi[0] >60)         desired_posi[0] = 60;
    else if(desired_posi[0] < -60)  desired_posi[0] = -60;

//    desired_posi[1] =  -(double)pEnc->m[0].nPos[1]*0.13;  //yaw
//    desired_posi[2] =  -(double)pEnc->m[0].nPos[3]*0.23; //roll
//    desired_posi[3] =  -(double)pEnc->m[0].nPos[0]*TIC_TO_LENGTH_TRANS; //translation
//    ROS_INFO("P: %f, Y: %f, R: %f, T: %f",desired_posi[0],desired_posi[1],desired_posi[2],desired_posi[3]);
}


/////////////2020 10 28 bdh
//////////////////////////////////////data save////////////////////////////////////////

void Thread2::data_save_trajectory_bending()
{
    int step_input =0;
    int image_case = 20;
    int hys_com    = 0;

    if(data_time>bending_range-1) //bending
    {
        desired_posi[0] = 0;
        send = false;
        stop();
    }

    step_input = bending[int(data_time)]+hys_com; // bending
    desired_posi[0] = -step_input; // robot move

    if(send && (step_input%2==0))
    {
       //ros image  saver pub
        da_msg::command saver_pub_msg;
        saver_pub_msg.parameter_value.push_back((double)image_case); //image
        saver_pub_msg.parameter_value.push_back(step_input-hys_com); //bending
        saver_pub_msg.start =1;
        image_save_pub.publish(saver_pub_msg);
        saver_pub_msg.parameter_value.clear();
        ROS_INFO("PUB data %d ",step_input);
    }

    //ROS_INFO("STEP INPUT %f, time= %f",(double)step_input,data_time);
    data_time += 0.02;


}

void Thread2::SCNN_data_save_traj()
{
    int step_input =0;

    if(data_time>bending_range-1) //bending
    {
        desired_posi[0] = 0;
        send = false;
        stop();
    }

    step_input = bending[int(data_time)]; // bending
    desired_posi[0] = step_input; // robot move

    if(send && (step_input%2==0))
    {
        if(desired_posi[0] == 0)
        {
           //ros image  saver pub
            da_msg::command saver_pub_msg;
            saver_pub_msg.parameter_value.push_back(desired_posi[0]); //bending
            saver_pub_msg.start =1;
            image_save_pub.publish(saver_pub_msg);
            saver_pub_msg.parameter_value.clear();
            ROS_INFO("PUB data %d ",desired_posi[0]);
        }
    }
    //ROS_INFO("STEP INPUT %f, time= %f",(double)step_input,data_time);
    data_time += 0.01;


}


}  // namespace da_robot
