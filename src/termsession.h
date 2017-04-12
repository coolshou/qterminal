#ifndef TERMSESSION_H
#define TERMSESSION_H

#include <QObject>
#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QSettings>
//#include <QMutexLocker>

#include "console.h"

#include <QDebug>

//class termsession : public QWidget
class termsession : public Console
{
    Q_OBJECT
public:
    explicit termsession(QWidget *parent = 0, QString name = "term", QSettings *settings = NULL);
    ~termsession();

    QString get_name();
    QString get_status();
    //return serial is open or not
    bool isOpen();
    void setConsole();
    void apply_setting();
    QVariant get_settingValue(QString key);
    void setLogDatetime(bool set);
    void setLogEnable(bool set);
    void setLogFilename(QString filename);
    QString getLogFileName();
    void logToFile(QByteArray log);
    void logToFile(QString lineToBelogged);
    void openSerialPort();
    void closeSerialPort();
    void writeln(const QByteArray &data);

signals:
    void sig_updateStatus(QString sMsg);
    void sig_updateActionBtnStatus(bool bStatus);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void readDataFromSerial();
    void writeDataToSerial(const QByteArray &data);

    void slot_handleError(QSerialPort::SerialPortError error);
    void slot_baudRateChanged(qint32 baudRate,QSerialPort::Directions directions);
    //void slot_onTextChanged();
private:
    QString getCurrentDateTimeString();

private:
    QSerialPort *serial;
    QString mGroupName;
    QSettings *mSetting;
    //log
    bool bLogEnable;
    QString sLogFilename;
    bool bLogDatetime;

};

#endif // TERMSESSION_H
