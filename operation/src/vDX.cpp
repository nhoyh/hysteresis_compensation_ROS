#include "../include/nobaek/vDX.h"
#include "stdio.h"
#include "qdebug.h"

vDX::vDX()
{
    bInit = false;

    // neutral position
    for(int port=0; port<MAX_PORT; port++)
        for(int motor=0; motor<MAX_MOTOR; motor++)
        {
            nTarget[port][motor] = 2048;
            dxl_present_position[motor] = 2048;
        }
}

vDX::~vDX()
{
    if(bInit)
    {
        for(int port=0; port<MAX_PORT; port++)
        {
            delete portHandler[port];
            delete groupSyncWrite[port];
        }
        delete packetHandler;
    }
}

int vDX::Init()
{
    if(!bInit)
    {
        for(int port=0; port<MAX_PORT; port++)
        {
            char portname[13];
            sprintf(portname, "/dev/ttyUSB%d", port);
            portHandler[port] = dynamixel::PortHandler::getPortHandler(portname);
        }
        packetHandler = dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);

        for(int port=0; port<MAX_PORT; port++)
            groupSyncWrite[port] = new dynamixel::GroupSyncWrite(portHandler[port], packetHandler, ADDR_XM_GOAL_POSITION, LEN_XM_GOAL_POSITION);

        for(int port=0; port<MAX_PORT; port++)
        {
            // Open port
            if (portHandler[port]->openPort() == false)
            {
                return -1;
            }
            else
            {
                // Set port baudrate
                if (portHandler[port]->setBaudRate(BAUDRATE) == false)
                {
                    return -2;
                }
            }
        }
        bInit = true;
        return 1;
    }
    return 0;
}

void vDX::Close()
{
    for(int port=0; port<MAX_PORT; port++)
        portHandler[port]->closePort();
}

int vDX::SetTorqueEnable(int port, int id, int onoff)
{
    int dxl_comm_result;

    // Enable Dynamixel Torque
    dxl_comm_result = packetHandler->write1ByteTxRx(portHandler[port], id, ADDR_XM_LED_ONOFF, onoff, &dxl_error);
    if (dxl_comm_result != COMM_SUCCESS)
        return -1;
    else if (dxl_error != 0)
        return -2;
    else
    {
        dxl_comm_result = packetHandler->write1ByteTxRx(portHandler[port], id, ADDR_XM_TORQUE_ENABLE, onoff, &dxl_error);
        if (dxl_comm_result != COMM_SUCCESS)
            return -3;
        else if (dxl_error != 0)
            return -4;
        else
            return 1;
    }
}




int vDX::SyncWrite(int port)
{
    uint8_t param_goal_position[MAX_MOTOR][4];

    for(int motor=0; motor<MAX_MOTOR; motor++)
    {
        // Allocate goal position value into byte array
        param_goal_position[motor][0] = DXL_LOBYTE(DXL_LOWORD(nTarget[port][motor]));
        param_goal_position[motor][1] = DXL_HIBYTE(DXL_LOWORD(nTarget[port][motor]));
        param_goal_position[motor][2] = DXL_LOBYTE(DXL_HIWORD(nTarget[port][motor]));
        param_goal_position[motor][3] = DXL_HIBYTE(DXL_HIWORD(nTarget[port][motor]));

        int nodeID = motor+1;
        groupSyncWrite[port]->addParam(nodeID, param_goal_position[motor]);
    }

    // Syncwrite goal position
    dxl_comm_result = groupSyncWrite[port]->txPacket();
    if (dxl_comm_result != COMM_SUCCESS)
        packetHandler->getTxRxResult(dxl_comm_result);

    // Clear syncwrite parameter storage
    groupSyncWrite[port]->clearParam();

    return 1;
}

int vDX::SetGoal(int port, int id, int target)
{
    packetHandler->write4ByteTxOnly(portHandler[port], id, ADDR_XM_GOAL_POSITION, target);
}

int vDX::ReadData(int port, int id)
{
    packetHandler->read4ByteTxRx(portHandler[port], id,ADDR_XM_PRESENT_POSITION,(uint32_t*)&dxl_present_position[id-1], &dxl_error);
//    packetHandler->read4ByteTxRx(portHandler[port], id,ADDR_XM_OPERATING_MODE,(uint32_t*)&dxl_present_position[id-1], &dxl_error);
    return dxl_present_position[id-1];

}




