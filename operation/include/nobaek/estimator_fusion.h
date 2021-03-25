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

#ifndef da_robot_THREAD4_HPP_
#define da_robot_THREAD4_HPP_

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
#include "da_msg/vision_estimation.h"

using namespace std;

namespace icra {

//thread
class Estimator : public QThread
{
    Q_OBJECT

    public:
        Estimator();
        void run();
        void scnn_pose_estimation_callback(const da_msg::vision_estimation::ConstPtr& msg);
        void optimization_callback(const da_msg::command::ConstPtr& msg);
        void aurora_callback(const da_msg::command::ConstPtr& msg);

        double estimated_angle, ave_angle;
        double boucwen_param[7];
        bool optimization_callback_on;

        bool aurora_callback_start;
        double aurora_pitch;

    private:

};


}  // namespace da_robot

#endif /* da_robot_QNODE_HPP_ */
