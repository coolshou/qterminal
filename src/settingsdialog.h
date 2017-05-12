/****************************************************************************
**
** Copyright (C) 2017 jimmy
**
****************************************************************************/

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QtSerialPort/QSerialPort>
#include <QCloseEvent>
#include <QSettings>
#include <QColor>
#include <QStandardPaths>
#include <QFont>
#include <QStringList>

#include "consoletheme.h"

QT_USE_NAMESPACE

QT_BEGIN_NAMESPACE

namespace Ui {
class SettingsDialog;
}

class QIntValidator;

QT_END_NAMESPACE

struct Settings
{
    //serial setting
    QString name;
    qint32 baudRate;
    QString stringBaudRate;
    QSerialPort::DataBits dataBits;
    QString stringDataBits;
    QSerialPort::Parity parity;
    QString stringParity;
    QSerialPort::StopBits stopBits;
    QString stringStopBits;
    QSerialPort::FlowControl flowControl;
    QString stringFlowControl;
    bool localEchoEnabled;
    //console setting
    int maxBlockCount;
    QString theme;
    QColor fontColor;
    QString fontFamily;
    int fontSize;
    int fontSizeDefault=10;
    ///The min. console font size.
    static const int MIN_FONT_SIZE = 4;
    ///The max. console font size.
    static const int MAX_FONT_SIZE = 20;
    QColor baseColor;
    bool scrollToBottom;
    //Log
    bool bLogEnable;
    QString stringLogFilename;
    bool bLogDateTime;

};

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

    Settings get_settings();
    void setSettings(QString gname, QSettings *settings);
    void setSettings(QString gname);

    bool isExistUsedSerial(QString serName);
    void addUsedSerial(QString serName);
    void delUsedSerial(QString serName);
    void updateUsedSerials(QStringList serials);

    void setCurrentTab(int idx);
    void setEditMode(bool edit);
    bool isEditMode();

protected:
    //void closeEvent(QCloseEvent event);
    void setDemo();
    void setDefaultSetting();

private:
    Q_SLOT void showPortInfo(int idx);
    Q_SLOT void apply();
    Q_SLOT void cancel();
    Q_SLOT void checkCustomBaudRatePolicy(int idx);
    Q_SLOT void checkCustomDevicePathPolicy(int idx);
    Q_SLOT void selectLogFileName();
    Q_SLOT void slot_changeFontSize(int size);
    Q_SLOT void changeTheme(QString themeName);
    Q_SLOT void changeBaseColor(QString color);
    Q_SLOT void changeFontColor(QString color);

    void fillPortsParameters();
    void fillPortsInfo();
    bool updateSettings();
    void fillConsoleParameters();

private:
    Ui::SettingsDialog *ui;
    bool editMode;
    Settings currentSettings;
    QIntValidator *intValidator;
    QString defaultGroupName;
    QStringList usedSerials; //record used serial port.
};

#endif // SETTINGSDIALOG_H
