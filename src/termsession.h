#ifndef TERMSESSION_H
#define TERMSESSION_H

#include <QObject>
#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QSettings>
//#include <QMutexLocker>

#include "console.h"
#include "script/scriptEngine.h"
#include "script/scriptThread.h"

#include <QDebug>

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
    //macro
    void macroStart();
    void macroStop();
    bool isMacroRunning();
    Qt::HANDLE getMacroThreadId();

signals:
    void sig_updateStatus(QString sMsg);
    void sig_updateActionBtnStatus(bool bStatus);
    Q_SIGNAL void scriptStarted(Qt::HANDLE);
    Q_SIGNAL void scriptFinished(Qt::HANDLE);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void readDataFromSerial();
    void writeDataToSerial(const QByteArray &data);

    void slot_handleError(QSerialPort::SerialPortError error);
    void slot_baudRateChanged(qint32 baudRate,QSerialPort::Directions directions);
    //void slot_onTextChanged();
    //macro
    void slot_scriptStarted();
    void slot_scriptFinished();

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
    //macro
    ScriptEngine *engine;
    scriptThread *worker;

};

#endif // TERMSESSION_H
