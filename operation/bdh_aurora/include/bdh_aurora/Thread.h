#ifndef THREAD_HMH
#define THREAD_HMH

#include <QThread>
#include <QMutex>
#include <QElapsedTimer>



class Thread : public QThread
{
    Q_OBJECT

public:
    Thread(void*);
    void    stop();
    double  nInterval;



public:

protected:
    void run();

private:
    QElapsedTimer m_tm;
    QMutex mutex;
    volatile bool bRun;
};

#endif
