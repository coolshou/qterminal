#include "termsession.h"
#include "settingsdialog.h"
#include <QMessageBox>
#include <QIcon>
#include <QDateTime>

#include <QFile>

#include "const.h"

termsession::termsession(QWidget *parent, QString name, QSettings *settings) :
    Console(parent)
{
    mGroupName = name;
    mSetting = settings;

    setConsole();

    serial = new QSerialPort(this);
    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)),
            this, SLOT(handleError(QSerialPort::SerialPortError)));
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(serial, SIGNAL(baudRateChanged(qint32,QSerialPort::Directions)),
            this, SLOT(slot_baudRateChanged(qint32,QSerialPort::Directions)));
    apply_setting();
}
termsession::~termsession()
{
    delete serial;
}
QString termsession::getLogFileName()
{
    return sLogFilename;
}
void termsession::logToFile(QByteArray log)
{
    QString myString(log);
    logToFile(myString);

}

void termsession::logToFile(QString lineToBelogged)
{
 //   QMutexLocker locker(&m_lineLoggerMutex);

    QFile f(getLogFileName());
    //doRollLogsIfNeeded(static_cast<qint64>(f.size() + lineToBelogged.length()));

    // Do not open in append mode but seek() to avoid warning for unseekable
    // devices, note that if open is made with WriteOnly without Append, the
    // file gets truncated
    if (!f.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QTextStream out(stdout);
        out << "CANNOT OPEN LOG FILE: " << getLogFileName();
        return;
    }
    // seek() does nothing on sequential devices, this is in essence what QFile
    // does when Append flag is set in open() but without warning (on Qt 4.8.3)
    // However, Qt 4.8.1 issues the warning, so check it explicitly
    if (!f.isSequential())
    {
        f.seek(f.size());
    }

    QTextStream out(&f);
    out << lineToBelogged;
}

QVariant termsession::get_settingValue(QString key)
{
    mSetting->beginGroup(mGroupName);
    return mSetting->value(key);
    mSetting->endGroup();
}

void termsession::apply_setting()
{
    mSetting->beginGroup(mGroupName);
    serial->setPortName(mSetting->value("name").toString());
    //serial->setBaudRate(mSetting->value("baudRate").value<QSerialPort::BaudRate>());
    serial->setBaudRate(mSetting->value("baudRate").toInt());
    serial->setDataBits(mSetting->value("dataBits").value<QSerialPort::DataBits>());
    serial->setParity(mSetting->value("parity").value<QSerialPort::Parity>());
    serial->setStopBits(mSetting->value("stopBits").value<QSerialPort::StopBits>());
    serial->setFlowControl(mSetting->value("flowControl").value<QSerialPort::FlowControl>());

    this->setEnabled(true);
    this->setLocalEchoEnabled(mSetting->value("localEchoEnabled").toBool());
    this->setScrollToBottom(mSetting->value("scrollToBottom").toBool());
    this->setLogDatetime(mSetting->value("logDateTime").toBool());
    this->setLogEnable(mSetting->value("logEnable").toBool());
    this->setLogFilename(mSetting->value("logFilename").toString());
    mSetting->endGroup();
}
void termsession::setLogDatetime(bool set)
{
    bLogDatetime = set;
}
void termsession::setLogEnable(bool set)
{
    bLogEnable = set;
}
void termsession::setLogFilename(QString filename)
{
    sLogFilename = filename;
}

void termsession::openSerialPort()
{
    apply_setting();
    if (serial->open(QIODevice::ReadWrite)) {
            emit sig_updateActionBtnStatus(false);
            //TODO:
            emit sig_updateStatus(get_status());
    } else {
        QMessageBox::critical(this, tr("Error"), serial->errorString());
        emit sig_updateStatus(tr("Open error"));
    }
}

void termsession::closeSerialPort()
{
    if (serial->isOpen())
        serial->close();
    this->setEnabled(false);
    emit sig_updateActionBtnStatus(true);
    emit sig_updateStatus(tr("Disconnected"));
}

void termsession::readData()
{
    QByteArray data = serial->readAll();
    if (bLogDatetime) {
        //qDebug() << data;
        //qDebug() << get_settingValue("logDateTime");
        QDateTime dateTime = dateTime.currentDateTime();
        QString dateTimeString = QString("\n[%1] ").arg(dateTime.toString("yyyy-MM-dd hh:mm:ss"));
        data.replace(QString("\n"), dateTimeString.toLatin1());
    }
    if (bLogEnable) {
        logToFile(data);
    }
    this->putData(data);
}
void termsession::writeData(const QByteArray &data)
{
    if (bLogEnable) {
        logToFile(data);
    }
    serial->write(data);
}
void termsession::writeln(const QByteArray &data)
{
    writeData(data);
    writeData("\r\n");
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
        mSetting->beginGroup(mGroupName);
        QString res = tr("Connected to %1 : %2, %3, %4, %5, %6")
                .arg(mSetting->value("name").toString())
                .arg(mSetting->value("baudRate").toString())
                .arg(mSetting->value("dataBits").toString())
                .arg(mSetting->value("parity").toString())
                .arg(mSetting->value("stopBits").toString())
                .arg(mSetting->value("flowControl").toString());
        mSetting->endGroup();
        return res;
    }
    return "";
}

bool termsession::isOpen()
{
    return serial->isOpen();
}
//console
void termsession::setConsole()
{
    this->showMaximized();
    connect(this, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));
    mSetting->beginGroup(mGroupName);
    this->setMaximumBlockCount(mSetting->value("maxBlockCount").toInt());
    this->setScrollToBottom(mSetting->value("scrollToBottom").toBool());
    mSetting->endGroup();
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
/*
void termsession::slot_onTextChanged()
{

}
*/
