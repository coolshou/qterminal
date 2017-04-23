/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia. For licensing terms and
** conditions see http://qt.digia.com/licensing. For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights. These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
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

protected:
    //void closeEvent(QCloseEvent event);
    void setDemo();
    void setDefaultSetting();

private slots:
    void showPortInfo(int idx);
    void apply();
    void cancel();
    void checkCustomBaudRatePolicy(int idx);
    void checkCustomDevicePathPolicy(int idx);
    void selectLogFileName();
    void slot_changeFontSize(int size);

private:
    void fillPortsParameters();
    void fillPortsInfo();
    bool updateSettings();
    void fillConsoleParameters();

private:
    Ui::SettingsDialog *ui;
    Settings currentSettings;
    QIntValidator *intValidator;
    QString defaultGroupName;
    QStringList usedSerials; //record used serial port.
};

#endif // SETTINGSDIALOG_H
