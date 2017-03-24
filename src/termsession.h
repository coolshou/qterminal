#ifndef TERMSESSION_H
#define TERMSESSION_H

#include <QObject>
#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QSettings>
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
    void new_console();
    void apply_setting();
    QVariant get_settingValue(QString key);
    void setLogDatetime(bool set);
    void setLogEnable(bool set);
    void setLogFilename(QString filename);

signals:
    void sig_updateStatus(QString sMsg);
    void sig_updateActionBtnStatus(bool bStatus);

public slots:
    void openSerialPort();
    void closeSerialPort();
    void writeln(const QByteArray &data);

private slots:
    void readData();
    void writeData(const QByteArray &data);

    void handleError(QSerialPort::SerialPortError error);
    void slot_baudRateChanged(qint32 baudRate,QSerialPort::Directions directions);

private:
    QSerialPort *serial;
    QString mGroupName;
    QSettings *mSetting;
    //TODO: log
    bool bLogEnable;
    QString sLogFilename;
    bool bLogDatetime;

};

#endif // TERMSESSION_H
