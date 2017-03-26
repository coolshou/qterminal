#ifndef CONST
#define CONST

#define MYORGDOMAIN "coolshou.idv"

#define MYWEBURL "http://www.coolshou.idv.tw"
#define MYSOURCEURL "https://github.com/coolshou/qtvt.git"
#define MYRELEASEURL "https://api.github.com/repos/coolshou/qtvt/releases/latest"

#include <QSerialPort>

Q_DECLARE_METATYPE(QSerialPort::DataBits)
Q_DECLARE_METATYPE(QSerialPort::Parity)
Q_DECLARE_METATYPE(QSerialPort::StopBits)
Q_DECLARE_METATYPE(QSerialPort::FlowControl)

#endif // CONST

