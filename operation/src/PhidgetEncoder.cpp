#include <stdio.h>
#include <math.h>
#include "../include/nobaek/PhidgetEncoder.h"


static int CCONV AttachHandler(CPhidgetHandle ENC, void *userptr);
static int CCONV DetachHandler(CPhidgetHandle ENC, void *userptr);
static int CCONV ErrorHandler(CPhidgetHandle ENC, void *userptr, int ErrorCode, const char *Description);
static int CCONV InputChangeHandler(CPhidgetEncoderHandle ENC, void *usrptr, int Index, int State);
static int CCONV PositionChangeHandler(CPhidgetEncoderHandle ENC, void *usrptr, int Index, int Time, int RelativePosition);
PhidgetEncoder *pPhidget;

PhidgetEncoder::PhidgetEncoder()
{
    //Declare an encoder handle
    for(int i=0; i<MAX_BOARD; i++)
    {
        hEncoder[i] = NULL;
        m[i].nPos[0] = 0.0;
        m[i].nPos[1] = 0.0;
        m[i].nPos[2] = 0.0;
        m[i].nPos[3] = 0;
        m[i].nID = 99999;
    }

    m[0].nSerialNo = 485052;
//    m[1].nSerialNo = 485427;
    //m[2].nSerialNo = 485052;
    //m[2].nSerialNo = 443239;

    bFootClutch[0] = false;
    bFootClutch[1] = false;
}

PhidgetEncoder::~PhidgetEncoder()
{

}

bool PhidgetEncoder::InitDevice()
{
    pPhidget = this;
    int result;
    const char *err;

    for(int i=0; i<MAX_BOARD; i++)
    {
        //create the encoder object
        CPhidgetEncoder_create(&hEncoder[i]);

        //Set the handlers to be run when the device is plugged in or opened from software,
        //unplugged or closed from software, or generates an error.
        CPhidget_set_OnAttach_Handler((CPhidgetHandle)hEncoder[i], AttachHandler, NULL);
        CPhidget_set_OnDetach_Handler((CPhidgetHandle)hEncoder[i], DetachHandler, NULL);
        CPhidget_set_OnError_Handler((CPhidgetHandle)hEncoder[i], ErrorHandler, NULL);

        //Registers a callback that will run if an input changes.
        //Requires the handle for the Phidget, the function that will be called, and an arbitrary pointer that will be supplied to the callback function (may be NULL).
        CPhidgetEncoder_set_OnInputChange_Handler(hEncoder[i], InputChangeHandler, NULL);

        //Registers a callback that will run if the encoder changes.
        //Requires the handle for the Encoder, the function that will be called, and an arbitrary pointer that will be supplied to the callback function (may be NULL).
        CPhidgetEncoder_set_OnPositionChange_Handler (hEncoder[i], PositionChangeHandler, NULL);

        CPhidget_open((CPhidgetHandle)hEncoder[i], -1);

        //get the program to wait for an encoder device to be attached
        //qDebug("Waiting for encoder to be attached....");
        if((result = CPhidget_waitForAttachment((CPhidgetHandle)hEncoder[i], 10000)))
        {
            CPhidget_getErrorDescription(result, &err);
            //qDebug("Problem occurred : %s\n", err);
            return false;
        }
    }

    return true;
}

void PhidgetEncoder::CloseDevice()
{
    for(int i=0; i<MAX_BOARD; i++)
    {
        CPhidget_close((CPhidgetHandle)hEncoder[i]);
        CPhidget_delete((CPhidgetHandle)hEncoder[i]);
    }
}

int CCONV AttachHandler(CPhidgetHandle ENC, void *userptr)
{
    int serialNo;
    CPhidget_DeviceID deviceID;
    int i, inputcount;

    CPhidget_getSerialNumber(ENC, &serialNo);

    for(int i=0; i<MAX_BOARD; i++)
    {
        if(serialNo == pPhidget->m[i].nSerialNo)
        {
            pPhidget->m[i].hBoard = (CPhidgetEncoderHandle)ENC;
            pPhidget->m[i].nID = i;
            break;
        }
    }

    //Retrieve the device ID and number of encoders so that we can set the enables if needed
    CPhidget_getDeviceID(ENC, &deviceID);
    CPhidgetEncoder_getEncoderCount((CPhidgetEncoderHandle)ENC, &inputcount);
    //qDebug("Encoder %10d attached! \n", serialNo);

    //the 1047 requires enabling of the encoder inputs, so enable them if this is a 1047
    if (deviceID == PHIDID_ENCODER_HS_4ENCODER_4INPUT)
    {
            //qDebug("Encoder requires Enable. Enabling inputs....\n");
            for (i = 0 ; i < inputcount ; i++)
                    CPhidgetEncoder_setEnabled((CPhidgetEncoderHandle)ENC, i, 1);
    }
    return 0;
}

int CCONV DetachHandler(CPhidgetHandle ENC, void *userptr)
{
    int serialNo;
    CPhidget_getSerialNumber(ENC, &serialNo);
    //qDebug("Encoder %10d detached! \n", serialNo);

    return 0;
}

int CCONV ErrorHandler(CPhidgetHandle ENC, void *userptr, int ErrorCode, const char *Description)
{
    //qDebug("Error handled. %d - %s \n", ErrorCode, Description);

    return 0;
}

int CCONV InputChangeHandler(CPhidgetEncoderHandle ENC, void *usrptr, int Index, int State)
{
    //qDebug("Input #%i - State: %i \n", Index, State);

    switch(Index)
    {
    case 0:  // left foot clutch
        if (State == 0)
            pPhidget->bFootClutch[0] = false;
        else if (State ==1)
            pPhidget->bFootClutch[0] = true;
        break;
    case 1: // right foot clutch
        if (State == 0)
            pPhidget->bFootClutch[1] = false;
        else if (State ==1)
            pPhidget->bFootClutch[1] = true;
        break;
    }

    return 0;
}

int CCONV PositionChangeHandler(CPhidgetEncoderHandle ENC, void *usrptr, int Index, int Time, int RelativePosition)
{
    for(int i=0; i<MAX_BOARD; i++)
    {
        if(ENC == pPhidget->m[i].hBoard)
        {
            int position;
            CPhidgetEncoder_getPosition(ENC, Index, &position);
            pPhidget->m[i].nPos[Index] = position;
           // qDebug("Encoder %d, %f \n", pPhidget->m[0].nPos[0]*(360/4096.0), (double)pPhidget->m[0].nPos[0]*(360/4096.0));
            break;
        }
    }


    return 0;
}


