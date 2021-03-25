#include "../include/nobaek/vaurora.h"
#include "stdio.h"
#include "unistd.h"
#include "../include/nobaek/mainwindow.h"

vAurora::vAurora(QObject *parent)
    : QObject(parent)
{
    m_port = new QSerialPort();
    m_port->setPortName("/dev/ttyUSB1");
    if(!m_port->open(QIODevice::ReadWrite))
    {
        qDebug("\n Serial port open error \n");
    }

    memset(m_cmd, 0, sizeof(m_cmd));
    memset(m_res, 0, sizeof(m_res));

    //m_pThread = new Thread(this);
    //bThread = false;
    bIsTracking = false;
    bTrackingMode = false;
    x = 0.0;    y = 0.0;        z = 0.0;
    yaw = 0.0;  pitch = 0.0;    roll = 0.0;
}

vAurora::~vAurora()
{
   if(bTrackingMode)
   {
       StopTracking();
   }
   //m_pThread->stop();
   //m_pThread->wait();
   //event->accept();
   //delete m_pThread;
   m_port->close();
   delete m_port;
}

void vAurora::OnReceived()
{


}

int vAurora::Init()
{
    // device restart
    if(Restart() != 1)
        return -1;

    // aurora communication setting
    if(SetAuroraComm() != 1)
        return -2;

    // computer communication setting
    m_port->setBaudRate(QSerialPort::Baud115200);
    m_port->setDataBits(QSerialPort::Data8);
    m_port->setParity(QSerialPort::NoParity);
    m_port->setStopBits(QSerialPort::OneStop);
    m_port->setFlowControl(QSerialPort::HardwareControl);
    usleep(1000000);

    // device init
    if(SetDeviceInit() != 1)
        return -3;

    return 1;
}

int vAurora::Restart()
{
    m_port->sendBreak();
    if(SendMessage( m_cmd, true ))
    {
        int index=0;
        int count=0;
        m_buffer = "";
        while(1)
        {
            m_port->waitForReadyRead(100);
            m_buffer += m_port->readAll();
            if ((index = m_buffer.indexOf("RESET")) != -1)
            {
                m_port->flush();
                return 1;
            }
            count++;
            if(count > 50)
                return -1;
        }
    }
    return 0;
}

int vAurora::SetAuroraComm()
{
    memset(m_cmd, 0, sizeof(m_cmd));
    sprintf(m_cmd, "COMM 50001");
    if(SendMessage( m_cmd, true ))
    {
        int index=0;
        int count=0;
        m_buffer = "";
        while(1)
        {
            m_port->waitForReadyRead(100);
            m_buffer += m_port->readAll();
            if ((index = m_buffer.indexOf("OKAY")) != -1)
            {
                m_port->flush();
                return 1;
            }
            count++;
            if(count>50)
                return -1;
        }
    }
    return 0;
}

int vAurora::SetDeviceInit()
{
    memset(m_cmd, 0, sizeof(m_cmd));
    sprintf(m_cmd, "INIT ");
    if(SendMessage( m_cmd, true ))
    {
        int index=0;
        int count=0;
        m_buffer = "";
        while(1)
        {
            m_port->waitForReadyRead(100);
            m_buffer += m_port->readAll();
            if ((index = m_buffer.indexOf("OKAY")) != -1)
            {
                m_port->flush();
                return 1;
            }
            count++;
            if(count>50)
                return -1;
        }
    }
    return 0;
}

int vAurora::ActivatePorts()
{
    if (!FreePortHandles())
        return -1;

    if (!InitializeAllPorts())
        return -2;

    if (!EnableAllPorts())
        return -3;

    return 1;
}

int vAurora::FreePortHandles()
{
    int
        nNoHandles = 0,
        nHandle = 0,
        n = 0;
    char szHandleList[MAX_REPLY_MSG];

    // get all the handles that need freeing
    memset(m_cmd, 0, sizeof(m_cmd));
    sprintf( m_cmd, "PHSR 01" );
    if(SendMessage( m_cmd, true ))
    {
        int index=0;
        int count=0;
        m_buffer = "";
        while(1)
        {
            m_port->waitForReadyRead(100);
            m_buffer += m_port->readAll();
            if ((index = m_buffer.indexOf('\r')) != -1)
            {
                m_buffer[index] = CARRIAGE_RETURN;
                m_buffer[index+1] = '\0';
                m_port->flush();
                break;
            }
            count++;
            if(count>50)
                return -1;
        }
        QByteArray temp = m_buffer.toLocal8Bit();
        strcpy(szHandleList, temp.data());

        nNoHandles = uASCIIToHex(&szHandleList[n], 2);
        n+=2;
        for ( int i = 0; i < nNoHandles; i++ )
        {
            nHandle = uASCIIToHex( &szHandleList[n], 2 );
            memset(m_cmd, 0, sizeof(m_cmd));
            sprintf( m_cmd, "PHF %02X", nHandle);
            n+=5;
            if (!SendMessage( m_cmd, true ))
                return 0;

            m_dtHandleInformation[nHandle].HandleInfo.bInitialized = false;
            m_dtHandleInformation[nHandle].HandleInfo.bEnabled = false;
        }
        return 1;
    }
    return 0;
}

int vAurora::InitializeAllPorts()
{
    int
        nNoHandles = 0,
        nHandle = 0,
        n = 0;
    char szHandleList[MAX_REPLY_MSG];

    do
    {
        n = 0;
        // get the handles that need to be initialized
        memset(m_cmd, 0, sizeof(m_cmd));
        sprintf( m_cmd, "PHSR 02" );

        if (!SendMessage( m_cmd, true ))
            return -1;

        int index=0;
        int count=0;
        m_buffer = "";
        while(1)
        {
            m_port->waitForReadyRead(100);
            m_buffer += m_port->readAll();
            if ((index = m_buffer.indexOf('\r')) != -1)
            {
                m_buffer[index] = CARRIAGE_RETURN;
                m_buffer[index+1] = '\0';
                m_port->flush();
                break;
            }
            count++;
            if(count>50)
                return -2;
        }

        QByteArray temp = m_buffer.toLocal8Bit();
        strcpy(m_res, temp.data());
        sprintf( szHandleList, m_res );
        nNoHandles = uASCIIToHex( &m_res[n], 2 );
        n+=2;

        if ( nNoHandles > 0 )
        {
            for ( int i = 0; i < nNoHandles; i++ )
            {
                nHandle = uASCIIToHex( &szHandleList[n], 2 );
                if ( !GetPortInformation( nHandle ) )
                    return 0;

                if ( !m_dtHandleInformation[nHandle].HandleInfo.bInitialized )
                {
                    if (!InitializeHandle( nHandle ))
                        return 0;
                    n+=5;
                }
            }
        }
    }while( nNoHandles > 0 );

    return 1;
}

int vAurora::InitializeHandle(int nHandle)
{
    memset(m_cmd, 0, sizeof(m_cmd));
    sprintf( m_cmd, "PINIT %02X", nHandle );
    if ( !SendMessage( m_cmd, true ))
        return -1;

    int index=0;
    int count=0;
    m_buffer = "";
    while(1)
    {
        m_port->waitForReadyRead(100);
        m_buffer += m_port->readAll();
        if ((index = m_buffer.indexOf("OKAY")) != -1)
        {
            m_dtHandleInformation[nHandle].HandleInfo.bInitialized = true;
            return 1;
        }
        count++;
        if(count>50)
            return -2;
    }
    return 0;
}

int vAurora::EnableAllPorts()
{
    int
        nNoHandles = 0,
        nPortHandle = 0,
        n = 0;
    char
        szHandleList[MAX_REPLY_MSG];

    int index=0;
    int count=0;
        //m_nPortsEnabled = 0;
    // get all the ports that need to be enabled
    memset(m_cmd, 0, sizeof(m_cmd));
    sprintf( m_cmd, "PHSR 03" );

    if(SendMessage( m_cmd, true ))
    {
        m_buffer = "";
        while(1)
        {
            m_port->waitForReadyRead(100);
            m_buffer += m_port->readAll();
            if ((index = m_buffer.indexOf('\r')) != -1)
            {
                m_buffer[index] = CARRIAGE_RETURN;
                m_buffer[index+1] = '\0';
                m_port->flush();
                break;
            }
            count++;
            if(count>50)
                return -1;
        }

        QByteArray temp = m_buffer.toLocal8Bit();
        strcpy(m_res, temp.data());
        sprintf( szHandleList, m_res );
        nNoHandles = uASCIIToHex( &szHandleList[n], 2 );
        n+=2;

        for ( int i = 0; i < nNoHandles; i++ )
        {
            nPortHandle = uASCIIToHex( &szHandleList[n], 2 );
            memset(m_cmd, 0, sizeof(m_cmd));
            sprintf( m_cmd, "PENA %02X%c", nPortHandle, 'D' );
            n+=5;
            if (!SendMessage( m_cmd, true ))
                return 0;

            m_buffer = "";
            while(1)
            {
                m_port->waitForReadyRead(100);
                m_buffer += m_port->readAll();
                if ((index = m_buffer.indexOf('\r')) != -1)
                {
                    m_buffer[index] = CARRIAGE_RETURN;
                    m_buffer[index+1] = '\0';
                    m_port->flush();
                    break;
                }
                count++;
                if(count>50)
                    return -2;
            }

            GetPortInformation( nPortHandle );
            //m_nPortsEnabled++;
        }
        return 1;
    }
    return 0;
}

int vAurora::StartTracking()
{
    memset(m_cmd, 0, sizeof(m_cmd));
    sprintf( m_cmd, "TSTART " );

    if(SendMessage( m_cmd, true ))
    {
        int index=0;
        int count=0;
        m_buffer = "";
        while(1)
        {
            m_port->waitForReadyRead(100);
            m_buffer += m_port->readAll();
            if ((index = m_buffer.indexOf("OKAY")) != -1)
            {
                m_port->flush();
               // m_pThread->start();
                bThread = true;
                bTrackingMode = true;
                return 1;
            }
            count++;
            if(count>50)
                return -1;
        }
    }
    return 0;
}

int vAurora::StopTracking()
{
    bThread = false;
    usleep(500000);
    memset(m_cmd, 0, sizeof(m_cmd));
    sprintf( m_cmd, "TSTOP " );

    if(SendMessage( m_cmd, true ))
    {
        int index=0;
        int count=0;
        m_buffer = "";
        while(1)
        {
            m_port->waitForReadyRead(100);
            m_buffer += m_port->readAll();
            if ((index = m_buffer.indexOf("OKAY")) != -1)
            {
                m_port->flush();
                //m_pThread->stop();
                bTrackingMode = false;
                return 1;
            }
            count++;
            if(count>50)
                return -1;
        }
    }
    return 0;
}

int vAurora::Beep(int nBeeps)
{
    memset(m_cmd, 0, sizeof(m_cmd));
    sprintf( m_cmd, "BEEP %d", nBeeps );

    if(SendMessage( m_cmd, true ))
    {
        int index=0;
        int count=0;
        m_buffer = "";
        while(1)
        {
            m_port->waitForReadyRead(100);
            m_buffer += m_port->readAll();
            if ((index = m_buffer.indexOf("1D4C1")) != -1)
            {
                m_port->flush();
                return 1;
            }
            count++;
            if(count>50)
                return -1;
        }
    }
    return 0;
}

int vAurora::GetPortInformation(int nPortHandle)
{
    unsigned int
        uASCIIConv = 0;
    char *pszPortInformation;

    memset(m_cmd, 0, sizeof(m_cmd));
    sprintf( m_cmd, "PHINF %02X0025", nPortHandle );

    if ( SendMessage( m_cmd, true ) )
    {
        int index=0;
        int count=0;
        m_buffer = "";
        while(1)
        {
            m_port->waitForReadyRead(100);
            m_buffer += m_port->readAll();
            if ((index = m_buffer.indexOf('\r')) != -1)
            {
                m_buffer[index] = CARRIAGE_RETURN;
                m_buffer[index+1] = '\0';
                m_port->flush();
                break;
            }
            count++;
            if(count>50)
                return -1;
        }

        QByteArray temp = m_buffer.toLocal8Bit();
        strcpy(m_res, temp.data());
        pszPortInformation = m_res;

        strncpy( m_dtHandleInformation[nPortHandle].szToolType, pszPortInformation, 8 );
        m_dtHandleInformation[nPortHandle].szToolType[8] = '\0';
        pszPortInformation+=8;
        strncpy( m_dtHandleInformation[nPortHandle].szManufact, pszPortInformation, 12 );
        m_dtHandleInformation[nPortHandle].szManufact[12] = '\0';
        pszPortInformation+=12;
        strncpy( m_dtHandleInformation[nPortHandle].szRev, pszPortInformation, 3 );
        m_dtHandleInformation[nPortHandle].szRev[3] = '\0';
        pszPortInformation+=3;
        strncpy( m_dtHandleInformation[nPortHandle].szSerialNo, pszPortInformation, 8 );
        m_dtHandleInformation[nPortHandle].szSerialNo[8] = '\0';
        pszPortInformation+=8;
        uASCIIConv = uASCIIToHex( pszPortInformation, 2 );
        pszPortInformation+=2;
        m_dtHandleInformation[nPortHandle].HandleInfo.bToolInPort = ( uASCIIConv & 0x01 ? 1 : 0 );
        m_dtHandleInformation[nPortHandle].HandleInfo.bGPIO1 = ( uASCIIConv & 0x02 ? 1 : 0 );
        m_dtHandleInformation[nPortHandle].HandleInfo.bGPIO2 = ( uASCIIConv & 0x04 ? 1 : 0 );
        m_dtHandleInformation[nPortHandle].HandleInfo.bGPIO3 = ( uASCIIConv & 0x08 ? 1 : 0 );
        m_dtHandleInformation[nPortHandle].HandleInfo.bInitialized = ( uASCIIConv & 0x10 ? 1 : 0 );
        m_dtHandleInformation[nPortHandle].HandleInfo.bEnabled = ( uASCIIConv & 0x20 ? 1 : 0 );
        m_dtHandleInformation[nPortHandle].HandleInfo.bTIPCurrentSensing = ( uASCIIConv & 0x80 ? 1 : 0 );

        /// parse the part number 0x0004
        strncpy( m_dtHandleInformation[nPortHandle].szPartNumber, pszPortInformation, 20 );
        m_dtHandleInformation[nPortHandle].szPartNumber[20] = '\0';
        pszPortInformation+=20;

        //if( m_dtSystemInformation.nTypeofSystem != VICRA_SYSTEM &&
//            m_dtSystemInformation.nTypeofSystem != SPECTRA_SYSTEM )
        {
            pszPortInformation+=10;
            sprintf( m_dtHandleInformation[nPortHandle].szPhysicalPort, "%d", nPortHandle );
            strncpy( m_dtHandleInformation[nPortHandle].szPhysicalPort, pszPortInformation, 2 );
            /* EC-03-0071
            m_dtHandleInformation[nPortHandle].szPhysicalPort[2] = '\0';
             */
            pszPortInformation+=2;
            strncpy( m_dtHandleInformation[nPortHandle].szChannel, pszPortInformation, 2 );
            m_dtHandleInformation[nPortHandle].szChannel[2] = '\0';
            if ( !strncmp( m_dtHandleInformation[nPortHandle].szChannel, "01", 2 ) )
            {
                /* EC-03-0071
                strncat(m_dtHandleInformation[nPortHandle].szPhysicalPort, "-b", 2 );
                 */
                strncpy(&m_dtHandleInformation[nPortHandle].szPhysicalPort[2], "-b", 2 );
                for ( int i = 0; i < NO_HANDLES; i++ )
                {
                    if ( strncmp( m_dtHandleInformation[i].szPhysicalPort, m_dtHandleInformation[nPortHandle].szPhysicalPort, 4 ) &&
                         !strncmp( m_dtHandleInformation[i].szPhysicalPort, m_dtHandleInformation[nPortHandle].szPhysicalPort, 2 ) )
                        /* EC-03-0071
                        strncat(m_dtHandleInformation[i].szPhysicalPort, "-a", 2 );
                         */
                        strncpy(&m_dtHandleInformation[i].szPhysicalPort[2], "-a", 2 );
                }
            }
        }
    }

    return 1;
}

int vAurora::GetTXTransforms(bool bReturn0x0800Option)
{
    int
        nReplyMode = 0x0001,
        nNoHandles = 0,
        nHandle = 0;
    unsigned int
        unHandleStatus = 0,
        unSystemStatus = 0;
    bool
        bDisabled = false;
    char
        *pszTransformInfo = NULL;

    /* report in volume only or out of volume as well */
    nReplyMode = bReturn0x0800Option ? 0x0801 : 0x0001;

    memset(m_cmd, 0, sizeof(m_cmd));
    sprintf( m_cmd, "TX %04X", nReplyMode );

    if(SendMessage( m_cmd, true ))
    {
        int index=0;
        int count=0;
        m_buffer = "";
        while(1)
        {
            m_port->waitForReadyRead(100);
            m_buffer += m_port->readAll();
            if ((index = m_buffer.indexOf('\r')) != -1)
            {
                m_buffer[index] = CARRIAGE_RETURN;
                m_buffer[index+1] = '\0';
                m_port->flush();
                break;
            }
            count++;
            if(count>50)
                return -1;
        }
        QByteArray temp = m_buffer.toLocal8Bit();
        strcpy(m_res, temp.data());
        pszTransformInfo = m_res;

        // TX Parsing Routines
        nNoHandles = uASCIIToHex( pszTransformInfo, 2 );
        pszTransformInfo+=2;

        for ( int i = 0; i < nNoHandles; i++ )
        {
            nHandle = uASCIIToHex( pszTransformInfo, 2 );
            pszTransformInfo+=2;

            bDisabled = false;

            if ( strlen( pszTransformInfo ) < 18 )
                return 0;

            if( !strncmp( pszTransformInfo, "MISSING", 7 ) ||
                !strncmp( pszTransformInfo, "DISABLED", 8 ) ||
                !strncmp( pszTransformInfo, "UNOCCUPIED", 10 ))
            {
                if ( !strncmp( pszTransformInfo, "UNOCCUPIED", 10 ))
                {

                    m_dtHandleInformation[nHandle].Xfrms.ulFlags = TRANSFORM_UNOCCUPIED;
                    pszTransformInfo+=10;
                    bDisabled = true;
                }
                else if ( !strncmp( pszTransformInfo, "DISABLED", 8 ))
                {
                    m_dtHandleInformation[nHandle].Xfrms.ulFlags = TRANSFORM_DISABLED;
                    pszTransformInfo+=8;
                    bDisabled = true;
                }
                else
                {
                    m_dtHandleInformation[nHandle].Xfrms.ulFlags = TRANSFORM_MISSING;
                    pszTransformInfo+=7;
                }
                /*
                m_dtHandleInformation[nHandle].Xfrms.rotation.q0 =
                m_dtHandleInformation[nHandle].Xfrms.rotation.qx =
                m_dtHandleInformation[nHandle].Xfrms.rotation.qy =
                m_dtHandleInformation[nHandle].Xfrms.rotation.qz =
                m_dtHandleInformation[nHandle].Xfrms.translation.x =
                m_dtHandleInformation[nHandle].Xfrms.translation.y =
                m_dtHandleInformation[nHandle].Xfrms.translation.z =
                m_dtHandleInformation[nHandle].Xfrms.fError = BAD_FLOAT;
                */
            } /* if */
            else
            {
                m_dtHandleInformation[nHandle].Xfrms.ulFlags = TRANSFORM_VALID;

                if (!bExtractValue( pszTransformInfo, 6, 10000.,
                                    &m_dtHandleInformation[nHandle].Xfrms.rotation.q0 ) ||
                    !bExtractValue( pszTransformInfo + 6, 6, 10000.,
                                    &m_dtHandleInformation[nHandle].Xfrms.rotation.qx ) ||
                    !bExtractValue( pszTransformInfo + 12, 6, 10000.,
                                    &m_dtHandleInformation[nHandle].Xfrms.rotation.qy ) ||
                    !bExtractValue( pszTransformInfo + 18, 6, 10000.,
                                    &m_dtHandleInformation[nHandle].Xfrms.rotation.qz ) ||
                    !bExtractValue( pszTransformInfo + 24, 7, 100.,
                                    &m_dtHandleInformation[nHandle].Xfrms.translation.x ) ||
                    !bExtractValue( pszTransformInfo + 31, 7, 100.,
                                    &m_dtHandleInformation[nHandle].Xfrms.translation.y ) ||
                    !bExtractValue( pszTransformInfo + 38, 7, 100.,
                                    &m_dtHandleInformation[nHandle].Xfrms.translation.z ) ||
                    !bExtractValue( pszTransformInfo + 45, 6, 10000.,
                                    &m_dtHandleInformation[nHandle].Xfrms.fError ) )
                {
                    m_dtHandleInformation[nHandle].Xfrms.ulFlags = TRANSFORM_MISSING;
                    return 0;
                }
                else
                {
                    pszTransformInfo+=51;
                }
            }

            // get handle status...
            if ( !bDisabled )
            {
                unHandleStatus = uASCIIToHex( pszTransformInfo, 8 );
                pszTransformInfo+=8;
                m_dtHandleInformation[nHandle].HandleInfo.bToolInPort = ( unHandleStatus & 0x01 ? 1 : 0 );
                m_dtHandleInformation[nHandle].HandleInfo.bGPIO1 = ( unHandleStatus & 0x02 ? 1 : 0 );
                m_dtHandleInformation[nHandle].HandleInfo.bGPIO2 = ( unHandleStatus & 0x04 ? 1 : 0 );
                m_dtHandleInformation[nHandle].HandleInfo.bGPIO3 = ( unHandleStatus & 0x08 ? 1 : 0 );
                m_dtHandleInformation[nHandle].HandleInfo.bInitialized = ( unHandleStatus & 0x10 ? 1 : 0 );
                m_dtHandleInformation[nHandle].HandleInfo.bEnabled = ( unHandleStatus & 0x20 ? 1 : 0 );
                m_dtHandleInformation[nHandle].HandleInfo.bOutOfVolume = ( unHandleStatus & 0x40 ? 1 : 0 );
                m_dtHandleInformation[nHandle].HandleInfo.bPartiallyOutOfVolume = ( unHandleStatus & 0x80 ? 1 : 0 );
                m_dtHandleInformation[nHandle].HandleInfo.bDisturbanceDet = ( unHandleStatus & 0x200 ? 1 : 0 );
                m_dtHandleInformation[nHandle].HandleInfo.bSignalTooSmall = ( unHandleStatus & 0x400 ? 1 : 0 );
                m_dtHandleInformation[nHandle].HandleInfo.bSignalTooBig = ( unHandleStatus & 0x800 ? 1 : 0 );
                m_dtHandleInformation[nHandle].HandleInfo.bProcessingException = ( unHandleStatus & 0x1000 ? 1 : 0 );
                m_dtHandleInformation[nHandle].HandleInfo.bHardwareFailure = ( unHandleStatus & 0x2000 ? 1 : 0 );

                /*get frame number...*/
                m_dtHandleInformation[nHandle].Xfrms.ulFrameNumber =
                    uASCIIToHex( pszTransformInfo, 8 );
                pszTransformInfo+=8;
            } /* if */
            pszTransformInfo++; /*for the carriage return*/
        } /* for */

        unSystemStatus = uASCIIToHex( pszTransformInfo, 4 );
        /*
        m_dtSystemInformation.bCommunicationSyncError = ( unSystemStatus & 0x01 ? 1 : 0 );
        m_dtSystemInformation.bTooMuchInterference = ( unSystemStatus & 0x02 ? 1 : 0 );
        m_dtSystemInformation.bSystemCRCError = ( unSystemStatus & 0x04 ? 1 : 0 );
        m_dtSystemInformation.bRecoverableException = ( unSystemStatus & 0x08 ? 1 : 0 );
        m_dtSystemInformation.bHardwareFailure = ( unSystemStatus & 0x10 ? 1 : 0 );
        m_dtSystemInformation.bHardwareChange = ( unSystemStatus & 0x20 ? 1 : 0 );
        m_dtSystemInformation.bPortOccupied = ( unSystemStatus & 0x40 ? 1 : 0 );
        m_dtSystemInformation.bPortUnoccupied = ( unSystemStatus & 0x80 ? 1 : 0 );
        m_dtSystemInformation.bDiagnosticsPending = ( unSystemStatus & 0x100 ? 1 : 0 );
        m_dtSystemInformation.bTemperatureOutOfRange = ( unSystemStatus & 0x200 ? 1 : 0 );
        */
    }

    // reference tracking...apply at the end of all the parsing so that
    // it is all the latest xfrms being applied
    //ApplyXfrms();

    return 1;
}
