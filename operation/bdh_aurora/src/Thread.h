#ifndef THREAD_HMH
#define THREAD_HMH

#include <QThread>
#include <QMutex>
#include <QElapsedTimer>
#include "da_msg/command.h"

class Thread : public QThread
{
    Q_OBJECT

public:
    Thread(void*);
    void    stop();
    double  nInterval;
    bool aurora_pub_start;

    void    angle_calculate();

    double xyz_init[3],rpy_init[3];
    double xyz_result[3],rpy_result[3];
    double pitch_angle_result;
    double c,l;

    void aurora_pub_pose();

protected:
    void run();

private:
    QElapsedTimer m_tm;
    QMutex mutex;
    volatile bool bRun;
};

#endif

