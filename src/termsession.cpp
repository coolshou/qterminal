#include "termsession.h"
#include "settingsdialog.h"
#include <QMessageBox>
#include <QIcon>

#include "const.h"

termsession::termsession(QWidget *parent, QString name, QSettings *settings) : QWidget(parent)
{
    mGroupName = name;
    mSetting = settings;
    console = new Console;
    //console->setEnabled(false);
    console->showMaximized();
    connect(console, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));

    serial = new QSerialPort(this);
    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)),
            this, SLOT(handleError(QSerialPort::SerialPortError)));
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(serial, SIGNAL(baudRateChanged(qint32,QSerialPort::Directions)),
            this, SLOT(slot_baudRateChanged(qint32,QSerialPort::Directions)));
}
termsession::~termsession()
{
    //delete console;
    delete serial;
}
/*
QSerialPort termsession::get_serial()
{
    return serial;
}
Console termsession::get_console()
{
    return console;
}
*/
void termsession::openSerialPort()
{
    mSetting->beginGroup(mGroupName);
    //qDebug() << "PortName: " << mSetting->value("name").toString();
    serial->setPortName(mSetting->value("name").toString());
    //serial->setBaudRate(mSetting->value("baudRate").value<QSerialPort::BaudRate>());
    serial->setBaudRate(mSetting->value("baudRate").toInt());
    serial->setDataBits(mSetting->value("dataBits").value<QSerialPort::DataBits>());
    serial->setParity(mSetting->value("parity").value<QSerialPort::Parity>());
    serial->setStopBits(mSetting->value("stopBits").value<QSerialPort::StopBits>());
    serial->setFlowControl(mSetting->value("flowControl").value<QSerialPort::FlowControl>());
    if (serial->open(QIODevice::ReadWrite)) {
            console->setEnabled(true);
            console->setLocalEchoEnabled(mSetting->value("localEchoEnabled").toBool());

            emit sig_updateActionBtnStatus(false);
            //TODO:
            emit sig_updateStatus(get_status());
    } else {
        QMessageBox::critical(this, tr("Error"), serial->errorString());
        emit sig_updateStatus(tr("Open error"));
    }
    mSetting->endGroup();
}

void termsession::closeSerialPort()
{
    if (serial->isOpen())
        serial->close();
    console->setEnabled(false);
    emit sig_updateActionBtnStatus(true);
    emit sig_updateStatus(tr("Disconnected"));
}

void termsession::readData()
{
    QByteArray data = serial->readAll();
    qDebug() << data;
    console->putData(data);
}
void termsession::writeData(const QByteArray &data)
{
    serial->write(data);
}

void termsession::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }
}

QString termsession::get_name()
{
    return mGroupName;
}
QString termsession::get_status()
{
    if (isOpen()) {
        return tr("Connected to %1 : %2, %3, %4, %5, %6")
                .arg(mSetting->value("name").toString())
                .arg(mSetting->value("baudRate").toString())
                .arg(mSetting->value("dataBits").toString())
                .arg(mSetting->value("parity").toString())
                .arg(mSetting->value("stopBits").toString())
                .arg(mSetting->value("flowControl").toString());
    }
    return "";
}

bool termsession::isOpen()
{
    return serial->isOpen();
}
void termsession::copy()
{
    console->copy();
}

void termsession::paste()
{
    console->paste();
}

void termsession::clear()
{
    console->clear();
}

void termsession::slot_baudRateChanged(qint32 baudRate,QSerialPort::Directions directions)
{
    if (serial->isOpen()) {
        closeSerialPort();
        qDebug() << "TODO: baudRate change:" << baudRate;
        openSerialPort();
    }
    Q_UNUSED(baudRate);
    Q_UNUSED(directions);
}
