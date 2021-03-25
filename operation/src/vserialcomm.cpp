/*
Description:	This class contains all the CRC calls needed
                through out the program, some of the following
                routines were taken from the API
*/

#include <stdio.h>
#include "../include/nobaek/vaurora.h"
#include <math.h>
//#include "conversions.h"

static unsigned int CrcTable[256];
static int bFirst=1;

int vAurora::SendMessage( char *m_szCommand, bool bAddCRC )
{
    unsigned int i;
    bool bComplete = false;

    // Check COM port
    if( m_port == NULL )
        return bComplete;

    // build the command, by adding a carraige return to it and crc if specified
    if (!BuildCommand( m_szCommand, bAddCRC ))
        return bComplete;

    if(strlen(m_szCommand) >= (MAX_COMMAND_MSG))
        return bComplete;

    for( i = 0; i < strlen(m_szCommand); i++ )
    {
        if ( m_port->putChar( m_szCommand[i] ) <= 0 )
        {
            bComplete = false;
            break;
        } /* if */
        else if( m_szCommand[i] == 0x0d )
        {
            bComplete = true;
            break;
        }
    }

    // flush the COM Port...this sends out any info still sitting in the buffer
    m_port->flush();

    return bComplete;
}

int vAurora::AddCRCToCommand( char * pszCommandString )
{
    int m,n;
    unsigned int uCrc;
    bool bFirstSpace = false;

    if(strlen(pszCommandString) >= (MAX_COMMAND_MSG-6))
        return 0;

    n=strlen(pszCommandString);
    // determine 16 bit CRC
    uCrc = 0;
    for(m=0;m<n;m++)
    {
        /*
         * replace space character with : if sending CRC
         * since parameter names can have spaces we need to
         * replace only the first space with the :
         */
        if(pszCommandString[m]==' ' && !bFirstSpace )
        {
            pszCommandString[m]=':';
            bFirstSpace = true;
        }
        uCrc = CalcCrc16(uCrc,pszCommandString[m]);
    } /* for */
    sprintf(&pszCommandString[n],"%04X",uCrc);
    n+=4;

    /*
     *Add the carriage return to the end
     */
    return 1;
}

int vAurora::AddCRToCommand( char * pszCommandString )
{
    int n;

    if(strlen(pszCommandString) >= (MAX_COMMAND_MSG-1))
        return 0;

    n=strlen(pszCommandString);
    pszCommandString[n++] = 0x0d;
    pszCommandString[n++] = '\0';
    return 1;
}

int vAurora::BuildCommand( char *pszCommandString, bool bAddCRC )
{
    int
        nCnt = 0,
        nNumCR = 0;

    // If the command is re-sent, it should already have a CR added.
    for ( nCnt = 0; nCnt < MAX_COMMAND_MSG; nCnt++)
    {
        if( pszCommandString[nCnt++] == 0x0d )
        {
            pszCommandString[nCnt++] = '\0';
            return 1;
        }/* if */
    }/* if */

    if ( bAddCRC )
        if (!AddCRCToCommand( pszCommandString ))
            return 0;

    if (!AddCRToCommand( pszCommandString ))
        return 0;

    return 1;
}

void vAurora::InitCrcTable()
{
    int i,j;
    long lCrcTable;
    /*
     * Create the CRC lookup table
     */
    for( i=0; i<256; i++ )
    {
        lCrcTable = i;
        for( j=0; j<8; j++ )
            lCrcTable = ( lCrcTable >> 1 ) ^ (( lCrcTable & 1 ) ? 0xA001L : 0 );

        CrcTable[i] = (unsigned int) lCrcTable & 0xFFFF;
    }
}

unsigned int vAurora::CalcCrc16( unsigned int crc, int data )
{
    if(bFirst) /* if this is the first time perform this */
    {
        bFirst = 0;
        InitCrcTable();
    } /* if */
    crc = CrcTable[ (crc ^ data) & 0xFF] ^ (crc >> 8);
    return (crc & 0xFFFF);
}

unsigned vAurora::CalcCRCByLen( char *pszString, int nLen )
{
static unsigned char
        oddparity[16] = { 0, 1, 1, 0, 1, 0, 0, 1,
                          1, 0, 0, 1, 0, 1, 1, 0 };
    unsigned
        data,
        uCrc = 0;
    unsigned char
        *puch = (unsigned char *)pszString;
    int
        nCnt = 0;

    while ( nCnt < nLen )
    {
        data = (*puch ^ (uCrc & 0xff)) & 0xff;
        uCrc >>= 8;

        if ( oddparity[data & 0x0f] ^ oddparity[data >> 4] )
        {
            uCrc ^= 0xc001;
        } /* if */

        data <<= 6;
        uCrc ^= data;
        data <<= 1;
        uCrc ^= data;
        puch++;
        nCnt++;
    } /* while */

    return uCrc;

}

int vAurora::SystemCheckCRC(char *psz)
{

    unsigned int
        uCrc = 0,
        uReplyCrc = 0,
        uReplySize = 0;

    int
        m, n;
    /*
     * calculate CRC
     */
    uCrc = 0;

    /*
     * We need to check if the reply is for BX, in binary format.
     * The start byte shall be 0xA5C4
     */
    if ( ((psz[0] & 0xff) == 0xc4) &&
         ((psz[1] & 0xff) == 0xa5) )
    {
        uReplyCrc = (psz[4] & 0xff) | ((psz[5] & 0xff) << 8); //get the header CRC

        if (CalcCRCByLen(psz, 4) == uReplyCrc) //Check the header CRC
        {
            /*
             *  Get the reply size.
             *  = reply size at [2] and [3] + 6 header bytes + 2 CRC bytes.
             */
            uReplySize = ((psz[2] & 0xff) | ((psz[3] & 0xff) << 8)) + 8;

            /* Get the body CRC */
            uReplyCrc = (psz[uReplySize-2] & 0xff) | ((psz[uReplySize-1] & 0xff) << 8);

            if (CalcCRCByLen(&psz[6], (uReplySize-8)) == uReplyCrc) // Check the CRC
            {
                return 1; /* CRC check OK */
            }
            else
            {
                return 0; /* Bad CRC */
            }/* else */
        }
        else
        {
            return 0; /* Bad CRC */
        }/* else */
    }
    else
    {
        //for( n = 0; (psz[n]!= CARRIAGE_RETURN) && (n< MAX_REPLY_MSG); n++)
        //{
            //; /* get total number of bytes n */
        //}/* for */

        /*
         * if rolled over the buffer size then something is wrong and
         * we will say the CRC is bad
         */
        //if(n>=MAX_REPLY_MSG)
          //  return 0;

        /*
         * NULL terminate the string to be tidy
         */
        psz[n+1] = 0;

        /*
         * determine 16 bit CRC
         */
        for(m=0;m<(n-4);m++)
                uCrc = CalcCrc16(uCrc,psz[m]);

        /*
         * read CRC from message
         */
        sscanf(&(psz[n-4]),"%04x",&uReplyCrc);

        /*
         * return the comparison of the calculated and received CRC values
         */
        return (uCrc == uReplyCrc);

    }/* else */
} /* SystemCheckCrc */

unsigned int vAurora::SystemGetCRC(char *psz, int nLength)
{
    unsigned int
        uCrc = 0;
    int
        m = 0;

    for(m=0; m<(nLength);m++)
    {
        uCrc = CalcCrc16(uCrc, psz[m]);
    }

    return uCrc;
}

unsigned int vAurora::uASCIIToHex( char szStr[], int nLen )
{
    char
        chTemp;
    unsigned int
        uVal;
    int
        nCnt;

    uVal = 0;
    for ( nCnt = 0; nCnt < nLen; ++nCnt )
    {
        chTemp = szStr[nCnt];
        /*
         * Convert hex ASCII digits to values to add to total value.
         */
        if ( (chTemp >= '0') && (chTemp <= '9') )
        {
            chTemp -= '0';
        } /* if */
        else if ( (chTemp >= 'A') && (chTemp <= 'F') )
        {
            chTemp = 0x0000000a + (chTemp - 'A');
        } /* else if */
        else if ( (chTemp >= 'a') && (chTemp <= 'f') )
        {
            chTemp = 0x0000000a + (chTemp - 'a');
        } /* else if */
        else
        {
            /*
             * We've hit a non-hex character.
             */
            return( 0 );
        } /* else */

        /*
         * Shift result into position of total value.
         */
        uVal |= (chTemp << (4 * (nLen - 1 - nCnt)));
    } /* for */

    return( uVal );
}

bool vAurora::bExtractValue( char *pszVal, unsigned uLen,
                    float fDivisor, float *pfValue )
{
    unsigned
        i;
    char
        szBuff[ 10 ];

    *pfValue = BAD_FLOAT;

    /*
     * Make sure that the first character is either a + or -.
     */
    if( *pszVal != '-' && *pszVal != '+' )
    {
      return false;
    } /* if */

    /*
     * Copy the + or - character to the buffer
     */
    szBuff[0] = *pszVal;

    /*
     * Copy the rest of the value.  Make sure that the remainder of
     * the value string contains only digits 0 - 9.
     */
    for( i = 1; i < uLen; i++ )
    {
        if( pszVal[i] < '0' || pszVal[i] > '9' )
        {
             return false;
        } /* if */

        szBuff[i] = pszVal[i];
    } /* for */

    /*
     * Null terminate the string.
     */
    szBuff[i] = '\0';

    *pfValue = float(atof( szBuff ) / fDivisor);

    return true;
}

void vAurora::CvtQuatToEulerRotation( QuatRotation *pdtQuatRot, Rotation *pdtEulerRot )
{
    RotationMatrix
        dtRotMatrix;

    CvtQuatToRotationMatrix( pdtQuatRot, dtRotMatrix );

    DetermineEuler( dtRotMatrix, pdtEulerRot );

    pdtEulerRot->fYaw *= RAD_TO_DEGREES;
    pdtEulerRot->fPitch *= RAD_TO_DEGREES;
    pdtEulerRot->fRoll *= RAD_TO_DEGREES;
}

void vAurora::CvtQuatToRotationMatrix( QuatRotation *pdtQuatRot, RotationMatrix dtRotMatrix )
{
    float
        fQ0Q0,
        fQxQx,
        fQyQy,
        fQzQz,
        fQ0Qx,
        fQ0Qy,
        fQ0Qz,
        fQxQy,
        fQxQz,
        fQyQz;

    /*
     * Determine some calculations done more than once.
     */
        fQ0Q0 = pdtQuatRot->q0 * pdtQuatRot->q0;
        fQxQx = pdtQuatRot->qx * pdtQuatRot->qx;
        fQyQy = pdtQuatRot->qy * pdtQuatRot->qy;
        fQzQz = pdtQuatRot->qz * pdtQuatRot->qz;
        fQ0Qx = pdtQuatRot->q0 * pdtQuatRot->qx;
        fQ0Qy = pdtQuatRot->q0 * pdtQuatRot->qy;
        fQ0Qz = pdtQuatRot->q0 * pdtQuatRot->qz;
        fQxQy = pdtQuatRot->qx * pdtQuatRot->qy;
        fQxQz = pdtQuatRot->qx * pdtQuatRot->qz;
        fQyQz = pdtQuatRot->qy * pdtQuatRot->qz;

    /*
     * Determine the rotation matrix elements.
     */
        dtRotMatrix[0][0] = fQ0Q0 + fQxQx - fQyQy - fQzQz;
        dtRotMatrix[0][1] = 2.0 * (-fQ0Qz + fQxQy);
        dtRotMatrix[0][2] = 2.0 * (fQ0Qy + fQxQz);
        dtRotMatrix[1][0] = 2.0 * (fQ0Qz + fQxQy);
        dtRotMatrix[1][1] = fQ0Q0 - fQxQx + fQyQy - fQzQz;
        dtRotMatrix[1][2] = 2.0 * (-fQ0Qx + fQyQz);
        dtRotMatrix[2][0] = 2.0 * (-fQ0Qy + fQxQz);
        dtRotMatrix[2][1] = 2.0 * (fQ0Qx + fQyQz);
        dtRotMatrix[2][2] = fQ0Q0 - fQxQx - fQyQy + fQzQz;
}

void vAurora::DetermineEuler( RotationMatrix dtRotMatrix, Rotation *pdtEulerRot )
{
    float
        fRoll,
        fCosRoll,
        fSinRoll;

    fRoll = atan2( dtRotMatrix[1][0], dtRotMatrix[0][0] );
    fCosRoll = cos( fRoll );
    fSinRoll = sin( fRoll );

    pdtEulerRot->fRoll = fRoll;
    pdtEulerRot->fPitch = atan2( -dtRotMatrix[2][0],
     (fCosRoll * dtRotMatrix[0][0]) + (fSinRoll *
     dtRotMatrix[1][0]) );
    pdtEulerRot->fYaw = atan2(
     (fSinRoll * dtRotMatrix[0][2]) -
     (fCosRoll * dtRotMatrix[1][2]),
     (-fSinRoll * dtRotMatrix[0][1]) +
     (fCosRoll * dtRotMatrix[1][1]) );

}
