#ifndef TERMSESSION_H
#define TERMSESSION_H

#include <QObject>
#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QSettings>
#include "console.h"


class termsession : public QWidget
{
    Q_OBJECT
public:
    explicit termsession(QWidget *parent = 0, QString name = "term", QSettings *settings = NULL);
    ~termsession();
    //
    Console *console;
    QSerialPort *serial;

    //QSerialPort get_serial();
    //Console get_console();
    QString get_name();

signals:
    void sig_updateStatus(QString sMsg);
    void sig_updateActionBtnStatus(bool bStatus);

public slots:
    void openSerialPort();
    void closeSerialPort();

private slots:
    void readData();
    void writeData(const QByteArray &data);
    void handleError(QSerialPort::SerialPortError error);


private:
    QString mGroupName;
    QSettings *mSetting;
    //MainWindow *mParent;
    //TODO: log
    bool bLogging;
    QString sLogFile;

};

#endif // TERMSESSION_H
