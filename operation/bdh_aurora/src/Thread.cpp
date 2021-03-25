#include "Thread.h"
#include "vaurora.h"
#include "QDebug"
#include <ros/ros.h>

#include "mainwindow.h"

#ifdef PI
    #undef PI
#endif
#define PI				3.14159265359
//math
#define sq(x)			(x*x)
#define PI                      (double)(3.14159265358979323846)
#define GRAVITY                 (double)(9.81)
#define DTR(x)                  (double)((double)(x) * PI / 180.0)	// degree to radian
#define RTD(x)                  (double)((double)(x) * 180.0 / PI)	// radian to degree

#define instrument_length 12
vAurora* pAurora;
ros::Subscriber aurora_start_receiver;
ros::Publisher aurora_pub;

Thread::Thread(void* pParent)
{    
    pAurora = (vAurora*)pParent;
    bRun = false;
    nInterval = 0;
    aurora_pub_start = false;

    for(int i=0;i<3;i++)
    {
        xyz_init[i] = 0.0;
        rpy_init[i] =0.0;
        xyz_result[i] = 0.0;
    }

    pitch_angle_result = 0.0;
    c = 0.0;
    l = 0.0;
}

void Thread::angle_calculate()
{

    if(pWnd->aurora_init == true)
    {
        xyz_init[0] = pAurora->x;
        xyz_init[1] = pAurora->y;
        xyz_init[2] = pAurora->z;
        rpy_init[0] = pAurora->roll;
        rpy_init[1] = pAurora->pitch;
        rpy_init[2] = pAurora->yaw;

        pWnd->aurora_init = false;
    }

    xyz_result[0] = pAurora->x - xyz_init[0];
    xyz_result[1] = pAurora->y - xyz_init[1];
    xyz_result[2] = pAurora->z - xyz_init[2];
    rpy_result[0] = pAurora->roll - rpy_init[0];
    rpy_result[1] = pAurora->pitch - rpy_init[1];
    rpy_result[2] = pAurora->yaw - rpy_init[2];

    //TO gui
    for(int i=0;i<3;i++) pWnd->aurora_cal_result[i] = xyz_result[i];
    for(int i=3;i<6;i++) pWnd->aurora_cal_result[i] = rpy_result[i-3];

    c = xyz_result[0]*xyz_result[0] + xyz_result[1]*xyz_result[1] + xyz_result[2]*xyz_result[2];
    l = double(instrument_length*instrument_length);


    if(xyz_result[0] >0) pitch_angle_result = -RTD(acos( 1 - (0.5*c/l) ));
    else pitch_angle_result = RTD(acos( 1 - (0.5*c/l) ));
    qDebug()<< pitch_angle_result <<","<<rpy_result[0];
    pWnd->pitch_result = pitch_angle_result;

}


void Thread::aurora_pub_pose()
{
    da_msg::command aurora_pub_pose_msg;
    aurora_pub_pose_msg.start =1;
//    aurora_pub_pose_msg.parameter_value.push_back(pAurora->x);
//    aurora_pub_pose_msg.parameter_value.push_back(pAurora->y);
//    aurora_pub_pose_msg.parameter_value.push_back(pAurora->z);
    aurora_pub_pose_msg.parameter_value.push_back(rpy_result[0]);
//    aurora_pub_pose_msg.parameter_value.push_back(pAurora->pitch);
//    aurora_pub_pose_msg.parameter_value.push_back(pAurora->yaw);
//    aurora_pub_pose_msg.parameter_value.push_back(pitch_angle_result);
    aurora_pub.publish(aurora_pub_pose_msg);
    aurora_pub_pose_msg.parameter_value.clear();
    qDebug() << "ros pub pitch angle ="<<pitch_angle_result;
}

void Thread::run()
{
    ros::start();
    ros::NodeHandle n;
    aurora_pub = n.advertise<da_msg::command>("aurora_pub_pose",1);

    m_tm.start();

    // thread control
    bool bIdle = false;
    qint64 nStart = 0.0;
    qint64 nEnd = nStart;
    int cnt=0;


    while (!bRun)
    {
        nEnd = m_tm.nsecsElapsed();
        if (nEnd-nStart < 10000000)    bIdle = true;    // measuring 10 (ms) loop
        else                          bIdle = false;

        if(!bIdle)
        {
            if(pAurora->bThread)
            {
                Rotation dtEulerRot;
                RotationMatrix dtRotMatrix;

                if ( !pAurora->GetTXTransforms( true ) )
                    break;

                for ( int i = 0; i < NO_HANDLES; i ++ )
                {
                    if ( pAurora->m_dtHandleInformation[i].HandleInfo.bInitialized == 1 )
                    {
                        if ( pAurora->m_dtHandleInformation[i].Xfrms.ulFlags == TRANSFORM_VALID )
                        {
                            pAurora->bIsTracking = true;
//                            if( i != pAurora->m_nRefHandle )
                            {
                                pAurora->x = pAurora->m_dtHandleInformation[i].Xfrms.translation.x;
                                pAurora->y = pAurora->m_dtHandleInformation[i].Xfrms.translation.y;
                                pAurora->z = pAurora->m_dtHandleInformation[i].Xfrms.translation.z;

                                pAurora->CvtQuatToEulerRotation( &pAurora->m_dtHandleInformation[i].Xfrms.rotation, &dtEulerRot );
                                pAurora->yaw = dtEulerRot.fYaw;
                                pAurora->pitch = dtEulerRot.fPitch;
                                pAurora->roll = dtEulerRot.fRoll;

                                pAurora->CvtQuatToRotationMatrix( &pAurora->m_dtHandleInformation[i].Xfrms.rotation, dtRotMatrix);

                                pAurora->m11 = dtRotMatrix[0][0];
                                pAurora->m12 = dtRotMatrix[0][1];
                                pAurora->m13 = dtRotMatrix[0][2];
                                pAurora->m21 = dtRotMatrix[1][0];
                                pAurora->m22 = dtRotMatrix[1][1];
                                pAurora->m23 = dtRotMatrix[1][2];
                                pAurora->m31 = dtRotMatrix[2][0];
                                pAurora->m32 = dtRotMatrix[2][1];
                                pAurora->m33 = dtRotMatrix[2][2];

                                /////////////////////////////////////////////////////
                                qDebug()<<"xyz" << pAurora->x << pAurora->y << pAurora->z <<pAurora->yaw <<pAurora->pitch <<pAurora->roll ;
                                qDebug()<<"cal" <<xyz_result[0]<<xyz_result[1]<<xyz_result[2]<<rpy_result[0]<<rpy_result[1]<<rpy_result[2];
                                qDebug()<<"pitch" << pitch_angle_result;
                                angle_calculate();
                                if(pWnd->aurora_pub) aurora_pub_pose();
                            }
                        }
                        else if ( pAurora->m_dtHandleInformation[i].Xfrms.ulFlags == TRANSFORM_MISSING )
                        {
                            pAurora->bIsTracking = false;
                        }
                    }
                }
            }
            nInterval = (nEnd-nStart)/10000000;  // control loop interval (ms)
            nStart = nEnd;
        }
    }
    bRun = false;
}

void Thread::stop()
{
    bRun = true;
}

