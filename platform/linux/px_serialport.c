#include "../modules/px_serialport.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include "dirent.h"

const char* PX_SerialPortEnumComName(int index)
{
    struct dirent* ent = 0;
    int count=0;
    DIR *pDir;
    static char COMName[260];

    if( (pDir=opendir("/dev")) == NULL)
    {
        int err=errno;
        return 0;
    }

    while( (ent=readdir(pDir)) != NULL )
    {
        if (count==index)
        {
            closedir(pDir);
            COMName[0]=0;
            strcpy(COMName, "/dev/");
            strcpy(COMName, ent->d_name);
            return COMName;
        }
    }
    closedir(pDir);
    return  0;
}
const int PX_SerialPortReset(PX_SerialPort *com,unsigned int baudRate,unsigned int DataBits,char ParityType,unsigned int stopBit)
{
    struct termios newtio;
    memset( &newtio, 0,sizeof( newtio ) );
    newtio.c_cflag  |=  CLOCAL | CREAD;
    newtio.c_cflag &= ~CSIZE;

    switch( DataBits )
    {
        case 5:
            newtio.c_cflag |= CS5;
            break;
        case 6:
            newtio.c_cflag |= CS6;
            break;
        case 7:
            newtio.c_cflag |= CS7;
            break;
        case 8:
            newtio.c_cflag |= CS8;
            break;
        default:
            break;
    }

    switch( ParityType )
    {
        case 'O':
            newtio.c_cflag |= PARENB;
            newtio.c_cflag |= PARODD;
            newtio.c_iflag |= (INPCK | ISTRIP);
            break;
        case 'E':
            newtio.c_iflag |= (INPCK | ISTRIP);
            newtio.c_cflag |= PARENB;
            newtio.c_cflag &= ~PARODD;
            break;
        case 'N':
            newtio.c_cflag &= ~PARENB;
            break;
    }

    switch( baudRate )
    {
        case 2400:
            cfsetispeed(&newtio, B2400);
            cfsetospeed(&newtio, B2400);
            break;
        case 4800:
            cfsetispeed(&newtio, B4800);
            cfsetospeed(&newtio, B4800);
            break;
        case 9600:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            break;
        case 115200:
            cfsetispeed(&newtio, B115200);
            cfsetospeed(&newtio, B115200);
            break;
        case 460800:
            cfsetispeed(&newtio, B460800);
            cfsetospeed(&newtio, B460800);
            break;
        default:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            break;
    }

    if( stopBit == 1 )
        newtio.c_cflag &=  ~CSTOPB;
    else if ( stopBit == 2 )
        newtio.c_cflag |=  CSTOPB;

    newtio.c_cc[VTIME]  =0;
    newtio.c_cc[VMIN] = 0;
    tcflush(com->Handle,TCIFLUSH);
    if((tcsetattr(com->Handle,TCSANOW,&newtio))!=0)
    {
        return 0;
    }
   return 1;
}
const int PX_SerialPortInitialize(PX_SerialPort *com,const char *name,unsigned int baudRate,unsigned int DataBits,char ParityType,unsigned int stopBit)
{

    com->BaudRate=baudRate;
    com->DataBits=DataBits;
    com->ParityType=ParityType;
    com->StopBit=stopBit;

    com->Handle = open(name, O_RDWR | O_NOCTTY);
    if(com->Handle < 0)
    {
        return 0;
    }

    if(!PX_SerialPortReset(com,baudRate,DataBits,ParityType,stopBit))
    {
        close(com->Handle);
        return  0;
    }
    return  1;

}

const int PX_SerialPortWrite(PX_SerialPort *com,void *data,int size)
{
    if (com->Handle!=0)
    {
        return write(com->Handle,data,size);
    }
    return  0;
}
const int PX_SerialPortRead(PX_SerialPort *com,void *data,int size)
{
    if (com->Handle!=0)
    {
        return read(com->Handle,data,size);
    }
    return  0;
}
void PX_SerialPortClose(PX_SerialPort *com)
{
    if (com->Handle!=0)
    {
        close(com->Handle);
    }
}