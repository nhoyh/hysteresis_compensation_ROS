#include "../include/nobaek/thread.h"
#include "../include/nobaek/mainwindow.h"
#include "time.h"

#define instrument_length 12

namespace icra{

Thread::Thread()
{
    bRun = false;
    nInterval = 0;

    for(int i=0;i<3;i++)
    {
        xyz_init[i] = 0.0;
        rpy_init[i] =0.0;
        xyz_result[i] = 0.0;
    }

    aurora_init = false;
    pitch_angle_result = 0.0;
    c = 0.0;
    l = 0.0;
}


void Thread::angle_calculate()
{

    if(aurora_init == true)
    {
        xyz_init[0] = pAurora->x;
        xyz_init[1] = pAurora->y;
        xyz_init[2] = pAurora->z;
        rpy_init[0] = pAurora->roll;
        rpy_init[1] = pAurora->pitch;
        rpy_init[2] = pAurora->yaw;

        aurora_init = false;
    }

    xyz_result[0] = pAurora->x - xyz_init[0];
    xyz_result[1] = pAurora->y - xyz_init[1];
    xyz_result[2] = pAurora->z - xyz_init[2];
    rpy_result[0] = pAurora->roll - rpy_init[0];
    rpy_result[1] = pAurora->pitch - rpy_init[1];
    rpy_result[2] = pAurora->yaw - rpy_init[2];

    c = xyz_result[0]*xyz_result[0] + xyz_result[1]*xyz_result[1] + xyz_result[2]*xyz_result[2];
    l = double(instrument_length*instrument_length);


    if(xyz_result[0] >0) pitch_angle_result = -RTD(acos( 1 - (0.5*c/l) ));
    else pitch_angle_result = RTD(acos( 1 - (0.5*c/l) ));

}

void Thread::run()
{
    m_tm.start();

    // thread control
    bool bIdle = false;
    qint64 nStart = 0.0;
    qint64 nEnd = nStart;

    while (!bRun)
    {
        nEnd = m_tm.nsecsElapsed();
        if (nEnd-nStart < 1000000)    bIdle = true;    // measuring 1 (ms) loop
        else                          bIdle = false;
        //qDebug("1");

        if(!bIdle)
        {
            if(pAurora->bThread)
            {
                //qDebug("2");

                Rotation dtEulerRot;
                RotationMatrix dtRotMatrix;

                if ( !pAurora->GetTXTransforms( true ) )
                    break;
                //qDebug("3");
                for ( int i = 0; i < NO_HANDLES; i ++ )
                {
                    if ( pAurora->m_dtHandleInformation[i].HandleInfo.bInitialized == 1 )
                    {
                        if ( pAurora->m_dtHandleInformation[i].Xfrms.ulFlags == TRANSFORM_VALID )
                        {
                            pAurora->bIsTracking = true;
                            //if( i != pAurora->m_nRefHandle )
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

                                angle_calculate();
                                //qDebug("4");

                            }
                        }
                        else if ( pAurora->m_dtHandleInformation[i].Xfrms.ulFlags == TRANSFORM_MISSING )
                        {
                            pAurora->bIsTracking = false;
                        }
//                        qDebug("%f, %f, %f",pAurora->x,pAurora->y,pAurora->z);

                    }
                }
            }
            nInterval = (nEnd-nStart)/1000000;  // control loop interval (ms)
            nStart = nEnd;
        }
    }
    bRun = false;
}

void Thread::stop()
{
    bRun = true;
}

};
