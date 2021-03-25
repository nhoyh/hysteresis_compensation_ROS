#ifndef PHIDGETENCODER_LDG
#define PHIDGETENCODER_LDG

#include "phidget21.h"
#include <QtDebug>

#define CPT_ENC1                (1000*4)
#define MAX_BOARD               1
#define Encoder_NUM             5
#define Master_DOF              4

struct module
{
   int nID;
   double nPos[Encoder_NUM];
   int master[Master_DOF];
   int nSerialNo;
   CPhidgetEncoderHandle hBoard;
};

class PhidgetEncoder
{
public:
    PhidgetEncoder();
    ~PhidgetEncoder();

    bool InitDevice();
    void CloseDevice();


    module m[MAX_BOARD];
    bool bFootClutch[2];
private:
    CPhidgetEncoderHandle hEncoder[MAX_BOARD];
};

#endif // PHIDGETENCODER_LDG
