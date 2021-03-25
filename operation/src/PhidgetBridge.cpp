#include "../include/nobaek/PhidgetBridge.h"


static int CCONV AttachHandler(CPhidgetHandle BRG, void *userptr);
static int CCONV DetachHandler(CPhidgetHandle BRG, void *userptr);
static int CCONV ErrorHandler(CPhidgetHandle BRG, void *userptr, int ErrorCode, const char *Description);
static int CCONV DataChangeHandler(CPhidgetBridgeHandle BRG, void *usrptr, int Index, double val);

PhidgetBridge *pPhidget_t;

PhidgetBridge::PhidgetBridge()
{
    //Declare an encoder handle
    for(int i=0; i<MAX_BOARD; i++)
    {
        hBridge[i] = 0;
        m[i].dData[0] = 0;
        m[i].dData[1] = 0;
        m[i].dData[2] = 0;
        m[i].dData[3] = 0;
        m[i].nID = 99999;
    }
    m[0].nSerialNo = 566756;

}

PhidgetBridge::~PhidgetBridge()
{

}

bool PhidgetBridge::InitDevice()
{
    pPhidget_t = this;
    int result;
    const char *err;

    for(int i=0; i<MAX_BOARD; i++)
    {
        //create the encoder object
        CPhidgetBridge_create(&hBridge[i]);
        //Set the handlers to be run when the device is plugged in or opened from software,
        //unplugged or closed from software, or generates an error.
        CPhidget_set_OnAttach_Handler((CPhidgetHandle)hBridge[i], AttachHandler, NULL);
        CPhidget_set_OnDetach_Handler((CPhidgetHandle)hBridge[i], DetachHandler, NULL);
        CPhidget_set_OnError_Handler((CPhidgetHandle)hBridge[i], ErrorHandler, NULL);

        //Registers a callback that will run if an input changes.
        //Requires the handle for the Phidget, the function that will be called, and an arbitrary pointer that will be supplied to the callback function (may be NULL).
        CPhidgetBridge_set_OnBridgeData_Handler(hBridge[i], DataChangeHandler, NULL);

        CPhidget_open((CPhidgetHandle)hBridge[i], -1);

        //get the program to wait for an encoder device to be attached
        qDebug("Waiting for encoder to be attached....");
        if((result = CPhidget_waitForAttachment((CPhidgetHandle)hBridge[i], 10000)))
        {
            CPhidget_getErrorDescription(result, &err);
            qDebug("Problem occurred : %s\n", err);
            return false;
        }
    }

    return true;
}

void PhidgetBridge::CloseDevice()
{
    for(int i=0; i<MAX_BOARD; i++)
    {
        CPhidget_close((CPhidgetHandle)hBridge[i]);
        CPhidget_delete((CPhidgetHandle)hBridge[i]);
    }
}

int CCONV AttachHandler(CPhidgetHandle BRG, void *userptr)
{
    int serialNo;
    CPhidget_DeviceID deviceID;
    int i, inputcount;

    CPhidget_getSerialNumber(BRG, &serialNo);

    for(int i=0; i<MAX_BOARD; i++)
    {
        if(serialNo == pPhidget_t->m[i].nSerialNo)
        {
            pPhidget_t->m[i].hBoard = (CPhidgetBridgeHandle)BRG;
            pPhidget_t->m[i].nID = i;
            break;
        }
    }

    //Retrieve the device ID and number of encoders so that we can set the enables if needed
    CPhidget_getDeviceID(BRG, &deviceID);
    CPhidgetBridge_getInputCount((CPhidgetBridgeHandle)BRG, &inputcount);

    //the 1047 requires enabling of the encoder inputs, so enable them if this is a 1047
    if (deviceID == PHIDID_BRIDGE_4INPUT)
    {
        //qDebug("Encoder requires Enable. Enabling inputs....\n");
        for (i = 0 ; i < inputcount ; i++)
        {
            CPhidgetBridge_setEnabled((CPhidgetBridgeHandle)BRG, i, PTRUE);
            CPhidgetBridge_setGain((CPhidgetBridgeHandle)BRG, i, PHIDGET_BRIDGE_GAIN_1);
        }
        CPhidgetBridge_setDataRate((CPhidgetBridgeHandle)BRG, DATA_RATE);
    }
    return 0;
}

int CCONV DetachHandler(CPhidgetHandle ENC, void *userptr)
{
    int serialNo;
    CPhidget_getSerialNumber(ENC, &serialNo);
    qDebug("Encoder %10d detached! \n", serialNo);

    return 0;
}

int CCONV ErrorHandler(CPhidgetHandle ENC, void *userptr, int ErrorCode, const char *Description)
{
    //qDebug("Error handled. %d - %s \n", ErrorCode, Description);

    return 0;
}

int CCONV DataChangeHandler(CPhidgetBridgeHandle BRG, void *usrptr, int Index, double val)
{
    CPhidgetBridgeHandle bridge = (CPhidgetBridgeHandle)BRG;
    for(int i=0; i<MAX_BOARD; i++)
    {
        if(bridge == pPhidget_t->m[i].hBoard)
        {
            if(Index==0)
                pPhidget_t->m[i].dData[Index] = val*62.57 - 0.1043;
            else if(Index ==1)
                pPhidget_t->m[i].dData[Index] = val*61.61 + 1.207;
            else if(Index ==2)
                pPhidget_t->m[i].dData[Index] = val*62.45 + 0.02476;
            else if(Index ==3)
                pPhidget_t->m[i].dData[Index] = val*59.89 - 0.06985;
            break;
        }

    }

    return 0;
}
