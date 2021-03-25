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

#ifndef THREAD1_LDG
#define THREAD1_LDG

/*****************************************************************************
** Includes
*****************************************************************************/
#include <ros/ros.h>
#include <string>
#include <QThread>
#include <QStringListModel>
#include <QMutex>
#include <QElapsedTimer>
#include <stdio.h>

#include "da_msg/command.h"

using namespace std;

namespace icra {

//thread
class Thread1 : public QThread
{
    Q_OBJECT

    public:
        Thread1();
        void    stop();
        double  nInterval;

        bool compensator_on;

        double vision_pose_estimation;
        double fusion_pose_estimation;

        double compensation_desired[2];
        double precompensated_value[2];

        double current_error[2];
        double current_error_dot[2];
        double error_sum[2];
        double pre_error[2];
        double feedback_value[2];

        double output_value[2];
        double pre_output_value[2];

        //function
        double feedback_compensator();

        double gain_[3];


    protected:
        void run();


    private:
        QElapsedTimer m_tm;
        QMutex mutex;
        volatile bool bRun;
};



}  // namespace da_robot

#endif /* THREAD1_LDG */
