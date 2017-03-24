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

#include "termsession.h"

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

protected:
    void closeEvent(QCloseEvent *event);

public slots:
    void updateStatus(QString sMsg);
    void updateActionBtnStatus(bool bStatus);
    void updateActionEditSessionBtnStatus(bool bStatus);
    int get_session_num();

private slots:
    void openSerialPort();
    void closeSerialPort();
    void sendSerialText();

    void showInputHistory(QString sInput);

    void about();
    void savePosSetting();
    void readPosSetting();
    void slot_acceptSettingDlg(int result);

    //actions
    //void on_action_Pop_Out_triggered();
    //file
    void add_session();
    void edit_session();
    void slot_closeSession();
    //edit
    void consoleCopy();
    void consolePaste();
    void consoleClear();
    QMdiSubWindow* get_currentSubWindow();
    termsession* get_termsession(QString sName);
    bool del_termsession(termsession* item);
    bool del_termsessionByName(QString sName);
    void slot_subWindowChanged(QMdiSubWindow* window);


private:
    void initActionsConnections();
    bool session_exist(QString sName);
    void updateMenuSession(bool state);
    void closeSession(QString sName);

private:
    Ui::MainWindow *ui;
    QSettings *settings;
    SettingsDialog *settingDlg;
    QList<termsession *> sessionlist;
};

#endif // MAINWINDOW_H
