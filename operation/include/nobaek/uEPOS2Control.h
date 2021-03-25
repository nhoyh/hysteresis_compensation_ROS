#ifndef UEPOS2CONTROL_LDG
#define UEPOS2CONTROL_LDG

#include "../include/Definitions.h"

#define OPEN_RS232 0x00
#define OPEN_CAN 0x01
#define OPEN_USB 0x02

#define POSITION_MODE   -1
#define VELOCITY_MODE   -2
#define CURRENT_MODE    -3
#define PROFILE_POSITION_MODE 1

class uEPOS2Control
{
public:
    uEPOS2Control();
    ~uEPOS2Control();

    int     InitDevice(char* pDeviceName, char* pProtocolStackName, char cControlMode, unsigned short st_node, unsigned short nb_node);
    bool    CloseDevice();
    int 	ClearFault(unsigned short m_wNodeId);
    int 	GetEnable(unsigned short m_wNodeId);
    int		SetEnable(unsigned short m_wNodeId, bool state);
    int 	SetGoal(unsigned short m_wNodeId, long nTarget);
    int		GetPosition(unsigned short m_wNodeId, int *nPosition);
    int		GetCurrent(unsigned short m_wNodeId, short *nCurrent);
    int		GetVelocity(unsigned short m_wNodeId, int *nVelocity);
    int     GetAnalogInput(unsigned short m_wNodeId, unsigned short *nAInput);
    int		SetMaxAcceleration(unsigned short m_wNodeId, unsigned long nMaxAcceleration);
    int		GetMaxAcceleration(unsigned short m_wNodeId, unsigned int* nMaxAcceleration);
    int		ResetEPOSDevice(unsigned short m_wNodeId);
    int     SetPositionProfile(unsigned short m_wNodeId, unsigned long ProfileVelocity, unsigned long ProfileAcceleration, unsigned long ProfileDeceleration);
    unsigned short  m_nStartNode;
    unsigned short  m_nEndNode;

protected:
    void*           m_hKeyHandle;
    unsigned int    m_nErrorCode;
    char            m_cControlMode;
};

#endif // UEPOS2CONTROL_LDG
