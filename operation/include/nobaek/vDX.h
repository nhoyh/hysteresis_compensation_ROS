#ifndef VDX_H
#define VDX_H

#include <dynamixel_sdk/dynamixel_sdk.h>

// Control table address
#define ADDR_XM_TORQUE_ENABLE          64
#define ADDR_XM_LED_ONOFF              65
#define ADDR_XM_GOAL_POSITION          116
#define ADDR_XM_PRESENT_POSITION       132
#define ADDR_XM_OPERATING_MODE         11

// Data byte length
#define LEN_XM_TORQUE_ENABLE            1
#define LEN_XM_LED_ONOFF                1
#define LEN_XM_GOAL_POSITION            4

// Protocol version
#define PROTOCOL_VERSION                2.0

#define MAX_PORT        1
#define MAX_MOTOR       4       // number of motor in each port
#define BAUDRATE        3000000

class vDX
{
public:
    vDX();
    ~vDX();
    int     Init();
    void    Close();
    int     SetTorqueEnable(int port, int id, int onoff);
    int     SyncWrite(int port);

    int     SetGoal(int port, int id, int target);
    int     ReadData(int port, int id);

    dynamixel::PortHandler *portHandler[MAX_PORT];
    dynamixel::PacketHandler *packetHandler;
    dynamixel::GroupSyncWrite *groupSyncWrite[MAX_PORT];
    uint8_t dxl_error;
    int dxl_comm_result;
    bool bInit;
    int nTarget[MAX_PORT][MAX_MOTOR];
    int32_t dxl_present_position[4];
};

#endif // VDX_H
