#ifndef da_robot_THREAD_HPP_
#define da_robot_THREAD_HPP_

#include <QThread>
#include <QMutex>
#include <QElapsedTimer>

#define yaw_motion 1
#define pitch_motion 0

using namespace std;

namespace icra {

    class Thread : public QThread
    {
        Q_OBJECT

    public:
        Thread();
        void    stop();
        double  nInterval;
        void    angle_calculate();

        bool   aurora_init;
        double xyz_init[3],rpy_init[3];
        double xyz_result[3],rpy_result[3];
        double pitch_angle_result;
        double c,l;

    protected:
       void run();

    private:
        QElapsedTimer m_tm;
        QMutex mutex;
        volatile bool bRun;
    };
}
#endif
