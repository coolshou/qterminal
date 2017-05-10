#ifndef CONST
#define CONST

#include <QtGlobal>

#define MYORGDOMAIN "coolshou.idv"

#define MYWEBURL "http://www.coolshou.idv.tw"
#define MYSOURCEURL "https://github.com/coolshou/qtvt.git"
#define MYRELEASEURL "https://api.github.com/repos/coolshou/qtvt/releases/latest"

#if defined(Q_OS_WIN)
    //Win32
    //#define AUTOSTARTFOLDER "/.config/autostart"
    //#define INSTALLDESKTOPFILE "/usr/share/applications/qtvt.desktop"
#elif defined(Q_OS_LINUX)
    //Linux
    #define AUTOSTARTFOLDER "/.config/autostart"
    #define INSTALLDESKTOPFILE "/usr/share/applications/qtvt.desktop"
#else
    #warn("Not supported SYSTEM")
#endif

#include <QSerialPort>

Q_DECLARE_METATYPE(QSerialPort::DataBits)
Q_DECLARE_METATYPE(QSerialPort::Parity)
Q_DECLARE_METATYPE(QSerialPort::StopBits)
Q_DECLARE_METATYPE(QSerialPort::FlowControl)

struct OSInfo
{
    QString cpuArch;
    QString productType;
    QString productVersion;
    QString account;
};

#endif // CONST

