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

#include "const.h"
#include <QDebug>

//! [0]
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //TODO: setting
    settings = new QSettings();


//    serial = new QSerialPort(this);
    /*
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionConfigure->setEnabled(true);
    */
    this->updateActionBtnStatus(true);
    ui->actionQuit->setEnabled(true);
    initActionsConnections();

}


MainWindow::~MainWindow()
{
    /*
    delete settingDlg;
    */
    delete ui;
    delete settings;

}


void MainWindow::openSerialPort()
{
    //TODO: open serial base on witch tab
    qDebug() <<"TODO: open serial base on witch tab";
    /*
    SettingsDialog::Settings p = settings->settings();
    serial->setPortName(p.name);
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);
    if (serial->open(QIODevice::ReadWrite)) {
            console->setEnabled(true);
            console->setLocalEchoEnabled(p.localEchoEnabled);
            ui->actionConnect->setEnabled(false);
            ui->actionDisconnect->setEnabled(true);
            ui->actionConfigure->setEnabled(false);
            ui->statusBar->showMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                                       .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                                       .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
    } else {
        QMessageBox::critical(this, tr("Error"), serial->errorString());

        ui->statusBar->showMessage(tr("Open error"));
    }
    */
}


void MainWindow::closeSerialPort()
{
    //TODO: close serial base on witch tab
    qDebug() <<"TODO: close serial base on witch tab";
    /*
    if (serial->isOpen())
        serial->close();
    console->setEnabled(false);
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionConfigure->setEnabled(true);
    ui->statusBar->showMessage(tr("Disconnected"));
    */
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About QTTerminal"),
                       tr("The <b>QTTerminal</b> to use the Qt Serial Port module"));
}


void MainWindow::initActionsConnections()
{
    //file
    //TODO: new
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(add_session()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    //cell
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(openSerialPort()));
    connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(closeSerialPort()));
    //edit

    //config
    //connect(ui->actionConfigure, SIGNAL(triggered()), settingDlg, SLOT(show()));
    //connect(ui->actionClear, SIGNAL(triggered()), console, SLOT(clear()));
    //window
    connect(ui->actionCascade, SIGNAL(triggered()), ui->mdiArea, SLOT(cascadeSubWindows()));
    connect(ui->actionTile, SIGNAL(triggered()), ui->mdiArea, SLOT(tileSubWindows()));
    //About
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
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
void MainWindow::add_session()
{
    //TODO: add a session (console) for mdi SubWindow
    //show console config dialog
    //setup new console for new SubWindow
    //
    qDebug() << "TODO: add_session";
    //termsession *termSession = new termsession(this, "term1" );
    termSession = new termsession(this, "term1" );
    //TODO: term setting?
    //TODO: delete termSession;
    connect(termSession, SIGNAL(sig_updateStatus(QString)), this, SLOT(updateStatus(QString)));
    connect(termSession, SIGNAL(sig_updateActionBtnStatus(bool)), this, SLOT(updateActionBtnStatus(bool)));
    //console = termSession->console;

    QMdiSubWindow *subwin1;
    subwin1 = ui->mdiArea->addSubWindow(termSession->console);
    //subwin1->setAttribute(Qt::WA_DeleteOnClose);
    //TODO: setting
    //settingDlg = new SettingsDialog;
    //subwin1->widget()->setWindowTitle(settingDlg->settings().name);
}

void MainWindow::edit_session()
{
    //TODO: edit session
    QMdiSubWindow *sw = ui->mdiArea->activeSubWindow();
    if (sw != 0 ) {
        //
        qDebug() << "edit_session";
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
    ui->actionConfigure->setEnabled(bStatus);
}
