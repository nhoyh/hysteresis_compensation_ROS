#include <ros/ros.h>
#include <ros/network.h>
#include <string>
#include <std_msgs/String.h>
#include <sstream>

#include "../include/nobaek/mainwindow.h"
#include "../include/nobaek/dy_packet.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"

namespace icra {

Thread3::Thread3()
{
    bRun = false;
    nInterval = 0;
    time = 0.0;
}

void Thread3::run()
{

    m_tm.start();

    // thread control
    bool bIdle = false;
    qint64 nStart = 0.0;
    qint64 nEnd = nStart;

    while (!bRun)
    {
        nEnd = m_tm.nsecsElapsed();
        if (nEnd-nStart < del_time)    bIdle = true;    // measuring 1 (ms) loop 33000000 del_time*1000
        else                          bIdle = false;

        if(!bIdle)
        {
            if(pWnd->bThread_flag)
            {
                 pMotor->nTarget[0][0] = pThread2->final_pos_value[YAW_1-1]; // id : 1
                 pMotor->nTarget[0][1] = pThread2->final_pos_value[PITCH_1-1];
                 pMotor->nTarget[0][2] = pThread2->final_pos_value[PITCH_2-1];
                 pMotor->nTarget[0][3] = pThread2->final_pos_value[YAW_2-1];
                 pMotor->SyncWrite(0);
                 usleep(1000);
            }

            nInterval = (nEnd-nStart)/(del_time);  // control loop interval (ms)
            nStart = nEnd;

        }
        else
        {
            usleep(1000);
        }
    }

    bRun = false;
}

void Thread3::stop()
{
    bRun = true;
}


}  // namespace da_robot
