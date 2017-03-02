#ifndef CONST
#define CONST

#define MYORG "coolshou"
#define MYORGDOMAIN "coolshou.idv"
#define MYAPP "qtvt"

#include <QSerialPort>

Q_DECLARE_METATYPE(QSerialPort::DataBits)
Q_DECLARE_METATYPE(QSerialPort::Parity)
Q_DECLARE_METATYPE(QSerialPort::StopBits)
Q_DECLARE_METATYPE(QSerialPort::FlowControl)

#endif // CONST

