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

#ifndef da_robot_THREAD3_HPP_
#define da_robot_THREAD3_HPP_

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

using namespace std;

namespace icra {

//thread
class Thread3 : public QThread
{
    Q_OBJECT

    public:
        Thread3();
        void    stop();
        double  nInterval;
        double  time;

    protected:
        void run();


    private:
        QElapsedTimer m_tm;
        QMutex mutex;
        volatile bool bRun;
};



}  // namespace da_robot

#endif /* da_robot_QNODE_HPP_ */
