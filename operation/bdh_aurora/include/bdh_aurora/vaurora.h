#ifndef VAURORA_H
#define VAURORA_H

// add "QT += core gui serialport" to project file

#define MAX_COMMAND_MSG     1024
#define MAX_REPLY_MSG       4096
#define MAX_BUFFER          1024
#define RAD_TO_DEGREES      (180 / 3.1415926)

#include <QSerialPort>
#include <QString>
#include <string.h>
#include <QObject>
#include "apistructures.h"
#include "Thread.h"



class vAurora : public QObject
{
    Q_OBJECT
public:
    explicit vAurora(QObject *parent = 0);
    ~vAurora();

    int Init();
    int Restart();
    int SetAuroraComm();
    int SetDeviceInit();
    int ActivatePorts();
    int FreePortHandles();
    int InitializeAllPorts();
    int GetPortInformation(int nPortHandle);
    int InitializeHandle(int nHandle);
    int EnableAllPorts();
    int StartTracking();
    int StopTracking();
    int Beep(int nBeeps);

    int SendMessage( char *m_szCommand, bool bAddCRC );

    void InitCrcTable();
    unsigned int CalcCrc16( unsigned int crc, int data );
    unsigned CalcCRCByLen( char *pszString, int nLen );
    int SystemCheckCRC(char *psz);
    unsigned int SystemGetCRC(char *psz, int nLength);
    unsigned int uASCIIToHex( char szStr[], int nLen );
    int AddCRCToCommand( char * pszCommandString );
    int AddCRToCommand( char * pszCommandString );
    int BuildCommand( char * pszCommandString, bool bAddCRC );
    int GetTXTransforms(bool bReturn0x0800Option);
    bool bExtractValue( char *pszVal, unsigned uLen, float fDivisor, float *pfValue );
    void CvtQuatToEulerRotation( QuatRotation *pdtQuatRot, Rotation *pdtEulerRot );
    void CvtQuatToRotationMatrix( QuatRotation *pdtQuatRot, RotationMatrix dtRotMatrix );
    void DetermineEuler( RotationMatrix dtRotMatrix, Rotation *pdtEulerRot );
    QSerialPort* m_port;
    HandleInformation m_dtHandleInformation[NO_HANDLES];	/* Handle Information varaible - structure */

    bool    bIsTracking;
    bool    bTrackingMode;
    bool    bThread;
    double  x,y,z;
    double  yaw,pitch,roll;
    double  m11,m12,m13;
    double  m21,m22,m23;
    double  m31,m32,m33;

private:
    int         m_state;
    QString     m_buffer;
    char        m_cmd[MAX_COMMAND_MSG];
    char        m_res[MAX_REPLY_MSG];
    Thread*     m_pThread;
    bool        bThread_flag;

private slots:
    void OnReceived();
};

#endif // VAURORA_H

