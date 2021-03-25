/**
 * @file /include/da_robot/qnode.hpp
 *
 * @brief Communications central!
 *
 * @date February 2011
 **/
/*****************************************************************************
** Ifdefs
*****************************************************************************/

#ifndef THREAD2_LDG
#define THREAD2_LDG

/*****************************************************************************
** Includes
*****************************************************************************/
#ifndef Q_MOC_RUN
#include <ros/ros.h>
#include <string>
#include <QThread>
#include <QStringListModel>
#include <QMutex>
#include <QElapsedTimer>
#include <stdio.h>

#endif

#define theta_dot_max 25
#define bending_motor_num 4
#define Num_of_joint 3


#define bending_range 70

using namespace std;


namespace icra {

//for tension controller
struct PID_module
{
    double desired_pre_tension[bending_motor_num];
    double tension_controller_output[bending_motor_num];
    double pre_tension_controller_output[bending_motor_num];
    double tension_err[bending_motor_num];
    double tension_err_sum[bending_motor_num];
    double tension_err_diff[bending_motor_num];

    double past_tension_err[bending_motor_num];
    double dtt;

    double P_[bending_motor_num],I_[bending_motor_num],D_[bending_motor_num],A_[bending_motor_num];
    double limit_pid;
};

struct FeedForward
{
    double des_pos_old;
    double H;
    double H_old;
    double des_vel;
    double H_dot;
    double dtt;

    double param[6];
};

//thread
class Thread2 : public QThread
{
    Q_OBJECT

    public:
        Thread2();
        void    stop();
        double  nInterval;
        double time, time_;

        double theta;
        double desired_posi[4];
        double pre_desired_posi[2], pre_desired_posi_dot[2];
        double desired_posi_dot[2];
        double desired_posi_ddot[2];

        double begin, end;

        double init_offset[4];
        double final_pos_value[4];

        double predirection_posi[2];
        double direction_vel[2];
        double predirection_vel[2];

        double Compensated_angle[2];
        double backlash_compensation[2];

        // LDG - loadcell variables
        double loadcell_tension[4];
        double motor_position[4];
        double loadcell1_ago[5];
        double loadcell2_ago[5];
        double loadcell3_ago[5];
        double loadcell4_ago[5];

        // LDG - motor current variables
        short pitch1_motor_current;
        short pitch2_motor_current;
        short yaw1_motor_current;
        short yaw2_motor_current;
        short* p_pitch1_motor_current = &pitch1_motor_current;
        short* p_pitch2_motor_current = &pitch2_motor_current;
        short* p_yaw1_motor_current = &yaw1_motor_current;
        short* p_yaw2_motor_current = &yaw2_motor_current;

        short motorcurrent1_ago[5];
        short motorcurrent2_ago[5];
        short motorcurrent3_ago[5];
        short motorcurrent4_ago[5];

        double LPF_motor_current[4];
        double LPF_alpha[4];

        //kinematics
        double La_d_1,La_d_2,j;
        double dLength[2];

        double R,B,d,L_a,L_b;
        double kinematic_output[2];

        double MAF_loadcell1(double loadcell1_cur_val);
        double MAF_loadcell2(double loadcell2_cur_val);
        double MAF_loadcell3(double loadcell3_cur_val);
        double MAF_loadcell4(double loadcell4_cur_val);

        double MAF_motor_current_pitch1(short motorcurrent_pitch1_cur_val);
        double MAF_motor_current_pitch2(short motorcurrent_pitch2_cur_val);
        double MAF_motor_current_yaw1(short motorcurrent_yaw1_cur_val);
        double MAF_motor_current_yaw2(short motorcurrent_yaw2_cur_val);

        //function
        void Inverse_kinematics_Model(double command, int dof);


        //trajectory
        double sinusoidal_trajectory();
        bool    traj_stop;
        double traj_calibration_func();
        bool conf_calibration;
        bool start_sin_traj;
        bool filter_on;
        int sin_magnitude;
        bool master_on;
        void tele_op_traj();

        //ros
        ros::Publisher optimization_start_pub;
        bool ik_pose_pub_start;
        ros::Publisher image_save_pub;
        ros::Publisher feedback_start_pub;


        //tension controller
        void tension_controller();
        PID_module pid_t;
        bool pre_tension_onoff;

        double  current_val[bending_motor_num];
        double  sensor_val[bending_motor_num];
        double  LPF_result[bending_motor_num];
        double  LPF_past[bending_motor_num];

        double  load_a[bending_motor_num];
        double  load_b[bending_motor_num];
        double  alpha;

        int configuration;
        bool compensation_end;

        double LPF_V_O[2];
        double LPF_V_P[2];


        bool feed_forward_on;
        bool feed_forward_start;
        double feed_forward_compensator(double des_th);
        FeedForward FF;

        void data_save_trajectory_bending();
        bool data_save_on;
        double data_time;
        bool send;
        double bending[bending_range];

        bool scnn_data_flag;
        void SCNN_data_save_traj();
        bool screenshot_;
        void pub_image_screen_shot();


    protected:
        void run();


    private:
        QElapsedTimer m_tm;
        QMutex mutex;
        volatile bool bRun;
};



}  // namespace da_robot

#endif /* THREAD2_LDG */
