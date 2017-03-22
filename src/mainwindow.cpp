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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "console.h"
#include "settingsdialog.h"

#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <QDialog>

#include "const.h"
#include <QDebug>

//! [0]
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //setting
    settings = new QSettings();
    readPosSetting();
    settingDlg = new SettingsDialog();
    connect(settingDlg, SIGNAL(finished(int)), this, SLOT(acceptSettingDlg(int)));

    connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(subWindowChanged(QMdiSubWindow*)));

    //this->updateActionBtnStatus(true);
    ui->actionQuit->setEnabled(true);
    initActionsConnections();

    get_session_num();
}


MainWindow::~MainWindow()
{
    /*
    delete settingDlg;
    */
    delete ui;
    delete settings;

}
void MainWindow::closeEvent(QCloseEvent *event)
{
    //TODO: ask quit?
    savePosSetting();
    event->accept();
}

void MainWindow::savePosSetting()
{
    settings->beginGroup("mainwindow");
    //settings->setValue("Geometry", saveGeometry());
    settings->setValue("pos", pos());
    if(!isMaximized())
        settings->setValue("size", size());
    settings->setValue("maximized", isMaximized());
    settings->setValue("State", saveState());

    settings->endGroup();
}
void MainWindow::readPosSetting()
{
    settings->beginGroup("mainwindow");
    //restoreGeometry(settings->value("Geometry").toByteArray());
    if(settings->contains("pos"))
        move(settings->value("pos").toPoint());
    if(settings->contains("size"))
        resize(settings->value("size").toSize());

    if(settings->value("maximized").toBool())
        setWindowState(windowState() | Qt::WindowMaximized);
    // create docks, toolbars, etc…
    restoreState(settings->value("State").toByteArray());
    settings->endGroup();
}

void MainWindow::acceptSettingDlg(int result)
{
    //qDebug() << "acceptSettingDlg: " << result;
    if (result == QDialog::Accepted) {
        qDebug() << "Accepted";
        QString sName = settingDlg->get_settings().name;
        if (session_exist(sName)){
            qDebug() << "TODO:acceptSettingDlg session_exist: " << sName;
            return;
        }
        settings->beginGroup(sName);
        settings->setValue("name", sName);
        settings->setValue("baudRate", settingDlg->get_settings().baudRate);
        settings->setValue("dataBits", settingDlg->get_settings().dataBits);
        settings->setValue("parity", settingDlg->get_settings().parity);
        settings->setValue("stopBits", settingDlg->get_settings().stopBits);
        settings->setValue("flowControl", settingDlg->get_settings().flowControl);
        settings->setValue("localEchoEnabled", settingDlg->get_settings().localEchoEnabled);
        //TODO: other console setting
        settings->setValue("maxBlockCount", settingDlg->get_settings().maxBlockCount);
        settings->setValue("baseColor", settingDlg->get_settings().baseColor);
        settings->setValue("fontColor", settingDlg->get_settings().fontColor);
        //TODO: Log
        settings->setValue("logEnable", settingDlg->get_settings().bLogEnable);
        settings->setValue("logFilename", settingDlg->get_settings().stringLogFilename);
        settings->setValue("logDateTime", settingDlg->get_settings().bLogDateTime);
        settings->endGroup();
        //TODO: check if serial is alweady exist?
        //setup new console for new SubWindow
        termsession *termSession = new termsession(this, sName, settings);
        sessionlist.append(termSession);
        //TODO: delete termSession;
        connect(termSession, SIGNAL(sig_updateStatus(QString)), this, SLOT(updateStatus(QString)));
        connect(termSession, SIGNAL(sig_updateActionBtnStatus(bool)), this, SLOT(updateActionBtnStatus(bool)));
        //console = termSession->console;

        QMdiSubWindow *subwin1 = new QMdiSubWindow;
        subwin1->setWidget(termSession->console);
        subwin1->setWindowIcon(QIcon(":/images/qtvt.png"));
        subwin1->setAttribute(Qt::WA_DeleteOnClose, false);
        subwin1->resize(QSize(ui->mdiArea->width(),ui->mdiArea->height()));
        subwin1->setWindowTitle(sName);
        ui->mdiArea->addSubWindow(subwin1);
        //ui->mdiArea->setOption(QMdiArea::DontMaximizeSubWindowOnActivation);//what this do for?
        subwin1->show();
        ui->mdiArea->setActiveSubWindow(subwin1);
    }
}
//open serial base on witch tab
void MainWindow::openSerialPort()
{
    //qDebug() <<"open serial base on witch tab";
    //QMdiSubWindow *sw = ui->mdiArea->currentSubWindow();
    QMdiSubWindow *sw = get_currentSubWindow();
    if ((sw == 0)||(sw == NULL)) {
        qDebug() << "openSerialPort not found subwindow";
        return;
    }
    qDebug() << "openSerialPort:" << sw->windowTitle();
    termsession *item = get_termsession(sw->windowTitle());
    item->openSerialPort();
    updateActionEditSessionBtnStatus(false);
}

void MainWindow::showInputHistory(QString sInput)
{
    qDebug() << "TODO: showInputHistory: " <<sInput;
}

void MainWindow::sendSerialText()
{
    QMdiSubWindow *sw = get_currentSubWindow();
    if ((sw == 0)||(sw == NULL)) {
        qDebug() << "sendSerialText not found subwindow";
        return;
    }
    termsession *item = get_termsession(sw->windowTitle());
    if (item->serial->isOpen()) {
        if (! ui->HistoryEdit->text().isEmpty()) {
            item->serial->write(ui->HistoryEdit->text().toLatin1());
        }
    }
}

//close serial base on witch tab
void MainWindow::closeSerialPort()
{
    //qDebug() <<"close serial base on witch tab";
    //QMdiSubWindow *sw = ui->mdiArea->currentSubWindow();
    QMdiSubWindow *sw = get_currentSubWindow();
    if ((sw == 0)||(sw == NULL)) {
        qDebug() << "closeSerialPort not found subwindow";
        return;
    }

    termsession *item = get_termsession(sw->windowTitle());
    if (item->serial->isOpen()) {
        item->closeSerialPort();
        updateActionEditSessionBtnStatus(true);
    }
}

void MainWindow::about()
{
    QString title =  QString("About %1").arg(MYAPP);
    QString about =  QString("The <b>%1</b> use the Qt Serial Port module <br>").arg(MYAPP);
    about.append(QString(" Author: %1").arg(MYORG));
    QMessageBox::about(this,title,about);
}


void MainWindow::initActionsConnections()
{
    //file - session
    connect(ui->actionNew_session, SIGNAL(triggered()), this, SLOT(add_session()));
    connect(ui->actionEdit_session, SIGNAL(triggered()), this, SLOT(edit_session()));
    connect(ui->actionClose_session, SIGNAL(triggered()), this, SLOT(close_session()));
    //TODO: log
    //connect(ui->actionLogFile, SIGNAL(triggered()), this, SLOT(logtofile()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    //cell
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(openSerialPort()));
    connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(closeSerialPort()));
    //edit
    connect(ui->actionCopy, SIGNAL(triggered()), this, SLOT(consoleCopy()));
    connect(ui->actionPaste, SIGNAL(triggered()), this, SLOT(consolePaste()));
    connect(ui->actionClear, SIGNAL(triggered()), this, SLOT(consoleClear()));
    //TODO: find
    //config
    //connect(ui->actionConfigure, SIGNAL(triggered()), settingDlg, SLOT(show()));

    //window
    connect(ui->actionCascade, SIGNAL(triggered()), ui->mdiArea, SLOT(cascadeSubWindows()));
    connect(ui->actionTile, SIGNAL(triggered()), ui->mdiArea, SLOT(tileSubWindows()));
    //About
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    //TODO: history
    connect(ui->HistoryEdit,SIGNAL(textChanged(QString)), this, SLOT(showInputHistory(QString)));
    connect(ui->SendBtn, SIGNAL(pressed()), this, SLOT(sendSerialText()));
}

//TODO: this is what I did from the main window to pop out a window.
/*
void MainWindow::on_action_Pop_Out_triggered()
{
    if (ui->mdiArea->activeSubWindow()){
        QMdiSubWindow *sub = ui->mdiArea->activeSubWindow();
        QWidget *wid = sub->widget();
        wid->hide();
        sub->deleteLater();
        ui->mdiArea->removeSubWindow(wid);
        wid->show();
    }
}
*/
bool MainWindow::session_exist(QString sName)
{
    QString tmp;
    //found if session name alweady exist
    foreach( termsession *item, sessionlist )
    {
        tmp = item->get_name();
        qDebug() << "name:" << tmp;
        if (QString::compare(sName, tmp, Qt::CaseInsensitive) == 0  ) {
            return true;
        }
    }
    return false;
}
QMdiSubWindow* MainWindow::get_currentSubWindow()
{
    return ui->mdiArea->currentSubWindow();
}

//get_termsession by name
termsession* MainWindow::get_termsession(QString sName)
{
    QString tmp;
    //found if session name alweady exist
    foreach( termsession *item, sessionlist )
    {
        tmp = item->get_name();
        //qDebug() << "get_termsession name:" << tmp;
        if (QString::compare(sName, tmp, Qt::CaseInsensitive) == 0  ) {
            return item;
        }
    }
    return NULL;
}
bool MainWindow::del_termsessionByName(QString sName)
{
    return del_termsession(get_termsession(sName));
}

bool MainWindow::del_termsession(termsession* item)
{
    return sessionlist.removeOne(item);
}

int MainWindow::get_session_num()
{
    //Get current session number
    int i;
    i= ui->mdiArea->subWindowList().count();
    return i;
}

//add a session (console) for mdi SubWindow
void MainWindow::add_session()
{
    //show console config dialog   
    settingDlg->exec(); //show as modal
}
//edit session
void MainWindow::edit_session()
{
    //QMdiSubWindow *sw = ui->mdiArea->activeSubWindow();
    QMdiSubWindow *sw = get_currentSubWindow();
    if ((sw != 0)||(sw != NULL)) {
        //qDebug() << "edit_session: " << sw->windowTitle();
        settingDlg->setSettings(sw->windowTitle(),settings);
        add_session();
    }
}
//close session
void MainWindow::close_session()
{
    //QMdiSubWindow *sw = ui->mdiArea->activeSubWindow();
    QMdiSubWindow *sw = get_currentSubWindow();
    if ((sw != 0)||(sw != NULL)) {
        //qDebug() << "close_session: " << sw->windowTitle();
        termsession *item = get_termsession(sw->windowTitle());
        item->closeSerialPort();
        item->close();
        del_termsession(item);
        ui->mdiArea->removeSubWindow(sw);
    }
}
void MainWindow::consoleCopy()
{
    QMdiSubWindow *sw = get_currentSubWindow();
    if ((sw != 0)||(sw != NULL)) {
        termsession *item = get_termsession(sw->windowTitle());
        item->copy();
    }
}
void MainWindow::consolePaste()
{
    QMdiSubWindow *sw = get_currentSubWindow();
    if ((sw != 0)||(sw != NULL)) {
        termsession *item = get_termsession(sw->windowTitle());
        item->paste();
    }
}

void MainWindow::consoleClear()
{
    QMdiSubWindow *sw = get_currentSubWindow();
    if ((sw != 0)||(sw != NULL)) {
        termsession *item = get_termsession(sw->windowTitle());
        item->clear();
    }
}

void  MainWindow::updateStatus(QString sMsg)
{
    ui->statusBar->showMessage(sMsg);
}

void MainWindow::updateActionBtnStatus(bool bStatus)
{
    ui->actionConnect->setEnabled(bStatus);
    ui->actionDisconnect->setEnabled(!bStatus);
    //ui->actionConfigure->setEnabled(bStatus);
    //updateActionConfigureBtnStatus(bStatus);
}
void MainWindow::updateActionEditSessionBtnStatus(bool bStatus)
{
    //current is connect
    ui->actionEdit_session->setEnabled(bStatus);
    ui->actionLogFile->setEnabled(bStatus);
}

void MainWindow::subWindowChanged(QMdiSubWindow* window)
{
    //When no SubWindow is Active QMdiSubWindow pointer is null
    //So,Check nullity here;otherwise application will crash
    if(window != NULL)
    {
        qDebug() << "TODO:(subWindowChanged) check the session is connected or not" ;
        termsession *item = get_termsession(window->windowTitle());
        updateStatus(item->get_status());

        updateMenuSession(true);

        //TODO: update status bar message by tab
        //TODO: update menu button status
      //setWindowTitle("Hello! Active SubWindow is : "+window->windowTitle());
    }
    else
    {
        updateMenuSession(false);
      //setWindowTitle("No active window");
    }
}

void MainWindow::updateMenuSession(bool state)
{
    //ui->actionEdit_session->setEnabled(state);
    updateActionEditSessionBtnStatus(state);
    ui->actionClose_session->setEnabled(state);
    ui->actionConnect->setEnabled(state);
}
