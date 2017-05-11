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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QtGlobal>

#include <QMainWindow>
#include <QMdiSubWindow>
#include <QtSerialPort/QSerialPort>
#include <QSettings>
#include <QCloseEvent>
#include <QStringList>
#include <QProcess>

#include "termsession.h"
#include "settingsdialog.h"
#include "updatedialog.h"
#include "optionsdialog.h"

#include "script/scriptEngine.h"
#include "script/scriptThread.h"

QT_BEGIN_NAMESPACE

namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE


class SettingsDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    Q_SLOT void receivedMessage( int instanceId, QByteArray message );
    QStringList getUsedSerial();
    Q_SLOT void updateStatus(QString sMsg);
    Q_SLOT void updateActionBtnStatus(bool bSerialConnected);
    Q_SLOT void updateActionEditSessionBtnStatus(bool bTermSessionExist);
    Q_SLOT int get_session_num();
    void detectSystem();
    void getAccountName();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Q_SLOT void openSerialPort();
    Q_SLOT void closeSerialPort();
    Q_SLOT void sendSerialText();

    Q_SLOT void showInputHistory(QString sInput);

    Q_SLOT void about();
    Q_SLOT void update();
    Q_SLOT void donate();
    Q_SLOT void savePosSetting();
    Q_SLOT void readPosSetting();
    Q_SLOT void slot_acceptSettingDlg(int result);

    //actions
    //void on_action_Pop_Out_triggered();
    //file
    Q_SLOT void add_session();
    Q_SLOT void edit_session();
    Q_SLOT void slot_closeSession();
    Q_SLOT void slot_logToFile();
    //edit
    Q_SLOT void consoleCopy();
    Q_SLOT void consolePaste();
    Q_SLOT void consoleClear();
    Q_SLOT void setScrollToBottom();
    Q_SLOT QMdiSubWindow* get_currentSubWindow();
    Q_SLOT termsession* get_termsession(QString sName);
    Q_SLOT bool del_termsession(termsession* item);
    Q_SLOT bool del_termsessionByName(QString sName);
    Q_SLOT void slot_subWindowChanged(QMdiSubWindow* window);
    //macro: each session may have it's own macro thread!!
    Q_SLOT void macroSetup();
    Q_SLOT void macroStart();
    Q_SLOT void macroStop();
    Q_SLOT void updateActionMacroBtnStatus(Qt::HANDLE id);
    Q_SLOT void updateActionMacroBtnStatus(bool bStatus);
    //option
    Q_SLOT void slot_options();
    Q_SLOT void slot_acceptOptionDlg(int result);

    //
    Q_SLOT void execFile(QString Filename);


    bool getCheckUpdateSetting();
    void initActionsConnections();
    void initToolBar();
    bool session_exist(QString sName);
    void updateMenuSession(bool state);
    //void updateActionStatus(bool state);
    void closeSession(QString sName);
    Q_SLOT void updateFontSizeSetting(int size);
    Q_SLOT void setAccountName(int code);

    void setAutoStart(bool start);
    void loadOptions();

private:
    Ui::MainWindow *ui;
    QSettings *settings;
    SettingsDialog *settingDlg;
    optionsDialog *optionDlg ;
    QList<termsession *> sessionlist;
    //updatedialog *updateDlg;
    OSInfo m_OSInof;
    QProcess *process;
    //options
    bool StartOnBoot;
    bool StartMinimal;
    bool RestortSession;
    bool CheckUpdate;
};

#endif // MAINWINDOW_H
