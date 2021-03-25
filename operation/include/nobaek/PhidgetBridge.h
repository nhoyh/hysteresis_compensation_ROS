 #ifndef PhidgetBridge_BDH
#define PhidgetBridge_BDH

#include "phidget21.h"
#include <QCoreApplication>
#include <QtDebug>

#define MAX_BOARD       1
#define DATA_RATE       50

struct module_t
{
    int nID;
    double dData[4];
    int nSerialNo;
    CPhidgetBridgeHandle hBoard;
};

class PhidgetBridge
{
public:
    PhidgetBridge();
    ~PhidgetBridge();

    bool InitDevice();
    void CloseDevice();

    module_t m[MAX_BOARD];

private:
    CPhidgetBridgeHandle hBridge[MAX_BOARD];
};

#endif // PhidgetBridge_HMH
