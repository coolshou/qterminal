#include "termsession.h"
#include "settingsdialog.h"
#include <QMessageBox>
#include <QIcon>

#include "const.h"

termsession::termsession(QWidget *parent, QString name, QSettings *settings) : QWidget(parent)
{
    //this->setWindowIcon(QIcon(":/images/qtvt.png")); not work?
    mGroupName = name;
    mSetting = settings;
    //mParent = parent;
    console = new Console;
    console->setEnabled(false);
    console->showMaximized();

    serial = new QSerialPort(this);

    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError)));
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(console, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));

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
    //TODO

    mSetting->beginGroup(mGroupName);
    //SettingsDialog::Settings p = settings->settings();
    serial->setPortName(mSetting->value("name").toString());
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
            emit sig_updateStatus(tr("Connected to %1 : %2, %3, %4, %5, %6")
                                  .arg(mSetting->value("name").toString())
                                  .arg(mSetting->value("baudRate").toString())
                                  .arg(mSetting->value("dataBits").toString())
                                  .arg(mSetting->value("parity").toString())
                                  .arg(mSetting->value("stopBits").toString())
                                  .arg(mSetting->value("flowControl").toString()));
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

