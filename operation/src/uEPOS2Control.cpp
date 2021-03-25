#include "../include/nobaek/uEPOS2Control.h"
#include "../include/nobaek/mainwindow.h"
#include <stdio.h>
#include <cstring>
#include <QDebug>

uEPOS2Control::uEPOS2Control()
{
    m_nStartNode = 0;
    m_nEndNode = 0;
    m_hKeyHandle = NULL;
    m_hKeyHandle = 0;
}

uEPOS2Control::~uEPOS2Control()
{

}

int uEPOS2Control::InitDevice(char* pDeviceName, char* pProtocolStackName, char cControlMode, unsigned short st_node, unsigned short nb_node)
{
    m_nStartNode = st_node;
    m_nEndNode = nb_node;
    char *pInterfaceName, *pPortName, *pProtocolStackName_;
    pInterfaceName = new char[5];
    pPortName = new char[5];
    pProtocolStackName_ = new char[20];

    // Open EPOS2 Device
    if(strcmp(pProtocolStackName, "RS232")==0)
    {
        strcpy(pProtocolStackName_, "MAXON_RS232");
        strcpy(pInterfaceName, "RS232");
        strcpy(pPortName, "COM1");
    }

    else if(strcmp(pProtocolStackName, "USB0")==0)
    {
        strcpy(pProtocolStackName_, "MAXON SERIAL V2");
        strcpy(pInterfaceName, "USB");
        strcpy(pPortName, "USB0");
    }

    else if(strcmp(pProtocolStackName, "USB1")==0)
    {
        strcpy(pProtocolStackName_, "MAXON SERIAL V2");
        strcpy(pInterfaceName, "USB");
        strcpy(pPortName, "USB1");
    }
    else if(strcmp(pProtocolStackName, "USB2")==0)
    {
        strcpy(pProtocolStackName_, "MAXON SERIAL V2");
        strcpy(pInterfaceName, "USB");
        strcpy(pPortName, "USB2");
    }
    else if(strcmp(pProtocolStackName, "USB3")==0)
    {
        strcpy(pProtocolStackName_, "MAXON SERIAL V2");
        strcpy(pInterfaceName, "USB");
        strcpy(pPortName, "USB3");
    }
    else if(strcmp(pProtocolStackName, "USB4")==0)
    {
        strcpy(pProtocolStackName_, "MAXON SERIAL V2");
        strcpy(pInterfaceName, "USB");
        strcpy(pPortName, "USB4");
    }
    else if(strcmp(pProtocolStackName, "CANOPEN")==0)
    {
        strcpy(pProtocolStackName_, "CANopen");
        strcpy(pInterfaceName, "IXXAT");
        strcpy(pPortName, "CAN0");
    }

    if ((m_hKeyHandle = VCS_OpenDevice(pDeviceName, pProtocolStackName_, pInterfaceName, pPortName, &m_nErrorCode)) == NULL)
        return -1;

    delete []pInterfaceName;
    delete []pPortName;
    delete []pProtocolStackName_;

    m_cControlMode = cControlMode;

    for(int i=m_nStartNode; i<=m_nEndNode; i++)
    {
        // Set Operation Mode
        if (VCS_SetOperationMode(m_hKeyHandle, i, m_cControlMode, &m_nErrorCode) == false)
            return -2;

        // Clear Fault
        if (VCS_ClearFault(m_hKeyHandle, i, &m_nErrorCode) == false)
            return -3;

        if (GetEnable(i) == true)   SetEnable(i,false);

        // 0x0000 : ST_DISABLED
        // 0x0001 : ST_ENABLED
        // 0x0002 : ST_QUICKSTOP
        // 0x0003 : ST_FAULT
        unsigned short State;
        VCS_GetState(m_hKeyHandle, i, &State, &m_nErrorCode);
        if(State != ST_DISABLED)
            return -4;
    }

    return 1;
}

bool uEPOS2Control::CloseDevice()
{
    for(int i=m_nStartNode; i<=m_nEndNode; i++)
    {
        if (GetEnable(i) == true)   SetEnable(i,false);

        // 0x0000 : ST_DISABLED
        // 0x0001 : ST_ENABLED
        // 0x0002 : ST_QUICKSTOP
        // 0x0003 : ST_FAULT
        unsigned short State;
        VCS_GetState(m_hKeyHandle, i, &State, &m_nErrorCode);
        if(State != ST_DISABLED)
            return false;
    }
    return true;
}

int uEPOS2Control::ClearFault(unsigned short m_wNodeId)
{
    if(m_wNodeId == 0)
    {
        for(int i=m_nStartNode; i<=m_nEndNode; i++)
            if (VCS_ClearFault(m_hKeyHandle, i, &m_nErrorCode) == false)
                return -1;
    }
    else
    {
        if (VCS_ClearFault(m_hKeyHandle, m_wNodeId, &m_nErrorCode) == false)
            return -2;
    }

    return 1;
}

int uEPOS2Control::SetEnable(unsigned short m_wNodeId, bool state)
{
    int nIsFault;
    if(m_wNodeId == 0)	// for all EPOS2
    {
        for(int i=m_nStartNode; i<=m_nEndNode; i++)
        {
            VCS_GetFaultState(m_hKeyHandle, i, &nIsFault, &m_nErrorCode);
            if (nIsFault==1)	ClearFault(i);
            if (state == true)
            {
                if (VCS_SetEnableState(m_hKeyHandle, i, &m_nErrorCode) == false)
                    return 0;
            }
            else
            {
                if (VCS_SetDisableState(m_hKeyHandle, i, &m_nErrorCode) == false)
                    return 0;
            }
        }
    }
    else
    {
        VCS_GetFaultState(m_hKeyHandle, m_wNodeId, &nIsFault, &m_nErrorCode);
        if (nIsFault==1)		ClearFault(m_wNodeId);
        if (state == true)
        {
            if (VCS_SetEnableState(m_hKeyHandle, m_wNodeId, &m_nErrorCode) == false)
                return 0;
        }
        else
        {
            if (VCS_SetDisableState(m_hKeyHandle, m_wNodeId, &m_nErrorCode) == false)
                return 0;
        }
    }

    return 1;
}

int uEPOS2Control::GetEnable(unsigned short m_wNodeId)
{
    int state;
    if (VCS_GetEnableState(m_hKeyHandle, m_wNodeId, &state, &m_nErrorCode) == false)
        return -1;

    if (state == 0 || state == 1)
        return state;
    else
        return -2;
}

int uEPOS2Control::GetPosition(unsigned short m_wNodeId, int *nPosition)
{
    if(VCS_GetPositionIs(m_hKeyHandle, m_wNodeId, nPosition, &m_nErrorCode) == false)
        return -1;

    return 1;
}

int uEPOS2Control::GetCurrent(unsigned short m_wNodeId, short *nCurrent)
{
    if(VCS_GetCurrentIs(m_hKeyHandle, m_wNodeId, nCurrent, &m_nErrorCode) == false)
        return -1;

    return 1;
}

int uEPOS2Control::GetVelocity(unsigned short m_wNodeId, int *nVelocity)
{
    if(VCS_GetVelocityIs(m_hKeyHandle, m_wNodeId, nVelocity, &m_nErrorCode) == false)
        return -1;

    return 1;
}

int uEPOS2Control::GetAnalogInput(unsigned short m_wNodeId, unsigned short *nAInput)
{
    if (VCS_GetAnalogInput(m_hKeyHandle, m_wNodeId, 0x01, nAInput, &m_nErrorCode) == false)
        return -1;

    return 1;
}

int uEPOS2Control::SetGoal(unsigned short m_wNodeId, long nTarget)
{
    switch (m_cControlMode)
    {
    case CURRENT_MODE:
        if (VCS_SetCurrentMust(m_hKeyHandle, m_wNodeId, nTarget, &m_nErrorCode) == false)
            return -1;
        break;
    case POSITION_MODE:
        if (VCS_SetPositionMust(m_hKeyHandle, m_wNodeId, nTarget, &m_nErrorCode) == false)
            return -1;
        break;
    case VELOCITY_MODE:
        if (VCS_SetVelocityMust(m_hKeyHandle, m_wNodeId, nTarget, &m_nErrorCode) == false)
            return -2;
        break;
    case PROFILE_POSITION_MODE:
        if (VCS_MoveToPosition(m_hKeyHandle, m_wNodeId, nTarget, true, true, &m_nErrorCode) == false)
            return -3;
        break;
    default:
        break;
    }

    return -4;
}

int uEPOS2Control::SetMaxAcceleration(unsigned short m_wNodeId, unsigned long nMaxAcceleration)
{
    // for all nodes
    if(m_wNodeId == 0)
    {
        for(int node=m_nStartNode; node<=m_nEndNode; node++)
        {
            if (VCS_SetMaxAcceleration(m_hKeyHandle, node, nMaxAcceleration, &m_nErrorCode) == false)
                return -1;
        }
    }
    else if(((m_wNodeId<m_nStartNode) || (m_wNodeId>m_nEndNode)) && (m_wNodeId != 0))
        return -2;
    else
    {
        if (VCS_SetMaxAcceleration(m_hKeyHandle, m_wNodeId, nMaxAcceleration, &m_nErrorCode) == false)
                return -3;
    }

    return 1;
}

int uEPOS2Control::GetMaxAcceleration(unsigned short m_wNodeId, unsigned int* nMaxAcceleration)
{
    if (VCS_GetMaxAcceleration(m_hKeyHandle, m_wNodeId, nMaxAcceleration, &m_nErrorCode) == false)
        return -1;

    return 1;
}



int uEPOS2Control::ResetEPOSDevice(unsigned short m_wNodeId)
{
    if(VCS_ResetDevice(m_hKeyHandle, m_wNodeId, &m_nErrorCode) == false)
        return -1;

    return 1;
}

int uEPOS2Control::SetPositionProfile(unsigned short m_wNodeId, unsigned long ProfileVelocity, unsigned long ProfileAcceleration, unsigned long ProfileDeceleration)
{
    // for all nodes
    if(m_wNodeId == 0)
    {
        for(int node=m_nStartNode; node<=m_nEndNode; node++)
        {
            if (VCS_SetPositionProfile(m_hKeyHandle, node, ProfileVelocity, ProfileAcceleration, ProfileDeceleration, &m_nErrorCode) == false)
                return -1;
        }
    }
    else if(((m_wNodeId<m_nStartNode) || (m_wNodeId>m_nEndNode)) && (m_wNodeId != 0))
        return -2;
    else
    {
        if (VCS_SetPositionProfile(m_hKeyHandle, m_wNodeId, ProfileVelocity, ProfileAcceleration, ProfileDeceleration, &m_nErrorCode) == false)
                return -3;
    }

    return 1;
}
