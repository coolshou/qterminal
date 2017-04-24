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


#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <QDialog>
#include <QProcess>
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
    connect(settingDlg, SIGNAL(finished(int)), this, SLOT(slot_acceptSettingDlg(int)));
    optionDlg = new optionsDialog();
    connect(optionDlg, SIGNAL(finished(int)), this, SLOT(slot_acceptOptionDlg(int)));

    connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(slot_subWindowChanged(QMdiSubWindow*)));

    //this->updateActionBtnStatus(true);
    ui->actionQuit->setEnabled(true);
    initActionsConnections();
    initToolBar();

    //get_session_num();
    //TODO: restore session/ start minial...
    //TODO: check for update
}


MainWindow::~MainWindow()
{
    delete settingDlg;
    delete ui;
    delete settings;
    delete optionDlg;
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    //TODO: ask quit?
    savePosSetting();
    event->accept();
}
bool MainWindow::getCheckUpdateSetting()
{
    bool b;
    settings->beginGroup("mainwindow");
    b = settings->value("CheckUpdate").toBool();
    settings->endGroup();
    return b;
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

void MainWindow::slot_acceptSettingDlg(int result)
{
    //qDebug() << "slot_acceptSettingDlg: " << result;
    if (result == QDialog::Accepted) {

        QString sName = settingDlg->get_settings().name;
        settings->beginGroup(sName);
        settings->setValue("name", sName);
        settings->setValue("baudRate", settingDlg->get_settings().baudRate);
        settings->setValue("dataBits", settingDlg->get_settings().dataBits);
        settings->setValue("parity", settingDlg->get_settings().parity);
        settings->setValue("stopBits", settingDlg->get_settings().stopBits);
        settings->setValue("flowControl", settingDlg->get_settings().flowControl);
        settings->setValue("localEchoEnabled", settingDlg->get_settings().localEchoEnabled);
        //console setting
        settings->setValue("maxBlockCount", settingDlg->get_settings().maxBlockCount);
        settings->setValue("baseColor", settingDlg->get_settings().baseColor);
        settings->setValue("fontColor", settingDlg->get_settings().fontColor);
        settings->setValue("fontFamily", settingDlg->get_settings().fontFamily);
        settings->setValue("fontSize", settingDlg->get_settings().fontSize);
        settings->setValue("scrollToBottom", settingDlg->get_settings().scrollToBottom);
        //Log
        settings->setValue("logEnable", settingDlg->get_settings().bLogEnable);
        settings->setValue("logFilename", settingDlg->get_settings().stringLogFilename);
        settings->setValue("logDateTime", settingDlg->get_settings().bLogDateTime);
        settings->endGroup();

        //check if serial is alweady exist
        if (session_exist(sName)){
            //qDebug() << "TODO:slot_acceptSettingDlg session_exist: " << sName;
            termsession *termSession = get_termsession(sName);
            //update setting in termSession
            termSession->apply_setting();
            return;
        } else {
            //setup new termsession for new SubWindow
            termsession *termSession = new termsession(this, sName, settings);
            sessionlist.append(termSession);
            connect(termSession, SIGNAL(sig_updateStatus(QString)), this, SLOT(updateStatus(QString)));
            connect(termSession, SIGNAL(sig_updateActionBtnStatus(bool)), this, SLOT(updateActionBtnStatus(bool)));
            connect(termSession, SIGNAL(scriptStarted(Qt::HANDLE)), this, SLOT(updateActionMacroBtnStatus(Qt::HANDLE)));
            connect(termSession, SIGNAL(scriptFinished(Qt::HANDLE)), this, SLOT(updateActionMacroBtnStatus(Qt::HANDLE)));
            connect(termSession, SIGNAL(fontSizeChanged(int)), this, SLOT(updateFontSizeSetting(int)));

            QMdiSubWindow *subwin1 = new QMdiSubWindow();
            //still show close button
            //subwin1->setWindowFlags(Qt::Window | Qt::WindowMinMaxButtonsHint);
            subwin1->setWidget(termSession);
            subwin1->setWindowIcon(QIcon(":/images/qtvt.png"));
            //subwin1->setAttribute(Qt::WA_DeleteOnClose, false);
            subwin1->resize(QSize(ui->mdiArea->width(),ui->mdiArea->height()));
            subwin1->setWindowTitle(sName);
            ui->mdiArea->addSubWindow(subwin1);
            //still show close button
            //ui->mdiArea->addSubWindow(subwin1, Qt::Window | Qt::WindowMinMaxButtonsHint);
            //what this do for?
            //ui->mdiArea->setOption(QMdiArea::DontMaximizeSubWindowOnActivation);
            subwin1->show();
            ui->mdiArea->setActiveSubWindow(subwin1);
        }
        openSerialPort();
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
    termsession *term = get_termsession(sw->windowTitle());
    term->openSerialPort();
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
    termsession *term = get_termsession(sw->windowTitle());
    if (term->isOpen()) {
        if (! ui->HistoryEdit->text().isEmpty()) {
            term->writeln(ui->HistoryEdit->text().toLatin1());
        }
    }
}

//close serial base on witch tab
void MainWindow::closeSerialPort()
{
    //TODO: close then open serial will cause problem??
    //qDebug() <<"close serial base on witch tab";
    //QMdiSubWindow *sw = ui->mdiArea->currentSubWindow();
    QMdiSubWindow *sw = get_currentSubWindow();
    if ((sw == 0)||(sw == NULL)) {
        qDebug() << "closeSerialPort not found subwindow";
        return;
    }

    termsession *term = get_termsession(sw->windowTitle());
    if (term->isOpen()) {
        term->closeSerialPort();
        updateActionEditSessionBtnStatus(true);
    }
}

void MainWindow::about()
{
    QString title =  QString("About %1").arg(APP_PRODUCT);
    QString about =  QString("The <b>%1</b> use the Qt Serial Port module <br>").arg(APP_PRODUCT);
    about.append(QString(" Author: %1<br>").arg(APP_COMPANY));
    about.append(QString(" Version: %1<br>").arg(APP_VERSION));
    about.append(QString(" Git Version: %1<br>").arg(GIT_VERSION));
    about.append(QString(" <a href='%1'>Source</a> <br>").arg(MYSOURCEURL));
    QMessageBox::about(this,title,about);
}

void MainWindow::update()
{
    updatedialog *updateDlg = new updatedialog(this);
    connect(updateDlg, SIGNAL(doExit()), this, SLOT(close()));
    connect(updateDlg, SIGNAL(doExec(QString)), this, SLOT(execFile(QString)));
    updateDlg->exec();//.exec();
    delete updateDlg;

}
void MainWindow::execFile(QString Filename)
{
    qDebug() <<"going to exec file: " << Filename ;
    QProcess *process = new QProcess(this);

    if (QSysInfo::productType() == "ubuntu") {
        QString file = "gdebi-gtk %1";
        process->startDetached(file.arg(Filename));
    } else if (QSysInfo::productType() == "windows") {
        qDebug() << "TODO: support launch windows setup";
    } else {
        qDebug() << "TODO: support to exec file at platform: " << QSysInfo::productType();
    }
}

void MainWindow::initToolBar()
{
    ui->toolBar->addAction(ui->actionClear);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->actionConnect);
    ui->toolBar->addAction(ui->actionDisconnect);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->actionScrollToBottom);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->actionMacroSetup);
    ui->toolBar->addAction(ui->actionMacroStart);
    ui->toolBar->addAction(ui->actionMacroStop);
    //TODO: initToolBar

}

void MainWindow::initActionsConnections()
{
    //menu
    //file - session
    connect(ui->actionNew_session, SIGNAL(triggered()), this, SLOT(add_session()));
    connect(ui->actionEdit_session, SIGNAL(triggered()), this, SLOT(edit_session()));
    connect(ui->actionClose_session, SIGNAL(triggered()), this, SLOT(slot_closeSession()));
    //file - log
    connect(ui->actionLogFile, SIGNAL(triggered()), this, SLOT(slot_logToFile()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    //cell
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(openSerialPort()));
    connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(closeSerialPort()));
    //edit
    connect(ui->actionCopy, SIGNAL(triggered()), this, SLOT(consoleCopy()));
    connect(ui->actionPaste, SIGNAL(triggered()), this, SLOT(consolePaste()));
    connect(ui->actionClear, SIGNAL(triggered()), this, SLOT(consoleClear()));
    connect(ui->actionScrollToBottom, SIGNAL(triggered()), this, SLOT(setScrollToBottom()));
    //TODO: actionFind
    //connect(ui->actionFind, SIGNAL(triggered()), this, SLOT(consoleFind()));
    //TODO: macro
    //connect(ui->actionMacro, SIGNAL(triggered()), this, SLOT(macro()));
    connect(ui->actionMacroSetup, SIGNAL(triggered()), this, SLOT(macroSetup()));
    connect(ui->actionMacroStart, SIGNAL(triggered()), this, SLOT(macroStart()));
    connect(ui->actionMacroStop, SIGNAL(triggered()), this, SLOT(macroStop()));
    //config
    connect(ui->actionOptions, SIGNAL(triggered()), this, SLOT(slot_options()));

    //window
    connect(ui->actionCascade, SIGNAL(triggered()), ui->mdiArea, SLOT(cascadeSubWindows()));
    connect(ui->actionTile, SIGNAL(triggered()), ui->mdiArea, SLOT(tileSubWindows()));
    //About
    //TODO: actionHelp
    //connect(ui->actionHelp, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionUpdate, SIGNAL(triggered()), this, SLOT(update()));
    //TODO: actionDonate
    //connect(ui->actionDonate, SIGNAL(triggered()), this, SLOT(update()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    //other
    //history input
    connect(ui->HistoryEdit,SIGNAL(textChanged(QString)), this, SLOT(showInputHistory(QString)));
    connect(ui->HistoryEdit,SIGNAL(returnPressed()), ui->SendBtn, SLOT(click()));
    connect(ui->SendBtn, SIGNAL(pressed()), this, SLOT(sendSerialText()));
}

//show options Dialog
void MainWindow::slot_options()
{
    if (optionDlg) {
        optionDlg->showSettings();
        optionDlg->exec();
    }
}
void MainWindow::slot_acceptOptionDlg(int result)
{
    if (result == QDialog::Accepted) {
        if (optionDlg) {
            optionDlg->applySettings();
        }
    }
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

QStringList MainWindow::getUsedSerial()
{
    QStringList ls;
    foreach( termsession *item, sessionlist )
    {
        qDebug() << "getUsedSerial: " << item->get_name();
        ls.append(item->get_name());
    }
    return ls;
}

//get_termsession by name
termsession* MainWindow::get_termsession(QString sName)
{
    QString tmp;
    //found if session name alweady exist
    foreach( termsession *item, sessionlist )
    {
        tmp = item->get_name();
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
    //do not show used serial port in settingDlg?
    //TODO: following will cause edit session problem!!
    //settingDlg->updateUsedSerials(getUsedSerial());
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
void  MainWindow::closeSession(QString sName)
{
    termsession *term = get_termsession(sName);
    term->closeSerialPort();
    term->close();
    del_termsession(term);
}

void MainWindow::slot_closeSession()
{
    QMdiSubWindow *sw = get_currentSubWindow();
    if ((sw != 0)||(sw != NULL)) {
        closeSession(sw->windowTitle());
        ui->mdiArea->removeSubWindow(sw);
    }
}
void MainWindow::slot_logToFile()
{
    //QMdiSubWindow *sw = ui->mdiArea->activeSubWindow();
    QMdiSubWindow *sw = get_currentSubWindow();
    if ((sw != 0)||(sw != NULL)) {
        //qDebug() << "edit_session: " << sw->windowTitle();
        settingDlg->setSettings(sw->windowTitle(),settings);
        settingDlg->setCurrentTab(1);
        add_session();
    }
}

void MainWindow::consoleCopy()
{
    QMdiSubWindow *sw = get_currentSubWindow();
    if ((sw != 0)||(sw != NULL)) {
        termsession *term = get_termsession(sw->windowTitle());
        term->copy();
    }
}
void MainWindow::consolePaste()
{
    QMdiSubWindow *sw = get_currentSubWindow();
    if ((sw != 0)||(sw != NULL)) {
        termsession *term = get_termsession(sw->windowTitle());
        term->paste();
    }
}

void MainWindow::consoleClear()
{
    QMdiSubWindow *sw = get_currentSubWindow();
    if ((sw != 0)||(sw != NULL)) {
        termsession *term = get_termsession(sw->windowTitle());
        term->clear();
    }
}
void MainWindow::setScrollToBottom()
{
    QMdiSubWindow *sw = get_currentSubWindow();
    if ((sw != 0)||(sw != NULL)) {
        termsession *term = get_termsession(sw->windowTitle());
        term->setScrollToBottom(!term->getScrollToBottom());
        settings->beginGroup(term->get_name());
        settings->setValue("scrollToBottom", term->getScrollToBottom());
        settings->endGroup();
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
    ui->actionScrollToBottom->setEnabled(!bStatus);
    //ui->actionConfigure->setEnabled(bStatus);
    //updateActionConfigureBtnStatus(bStatus);
}
void MainWindow::updateActionEditSessionBtnStatus(bool bStatus)
{
    //qDebug() << "updateActionEditSessionBtnStatus:" << bStatus;
    ui->actionClose_session->setEnabled(bStatus);
    //current is connect
    ui->actionEdit_session->setEnabled(bStatus);
    ui->actionLogFile->setEnabled(bStatus);
}

void MainWindow::slot_subWindowChanged(QMdiSubWindow* window)
{
    //When no SubWindow is Active QMdiSubWindow pointer is null
    //So, Check nullity here;otherwise application will crash
    if(window != NULL)
    {
        termsession *term = get_termsession(window->windowTitle());
        //update status bar message by tab
        updateStatus(term->get_status());
        //update menu button status
        updateMenuSession(true);
        updateActionBtnStatus(!term->isOpen());
    }
    else
    {
        updateMenuSession(false);
        updateActionBtnStatus(true);
    }
}

void MainWindow::updateMenuSession(bool state)
{
    //ui->actionEdit_session->setEnabled(state);
    updateActionEditSessionBtnStatus(state);
}
void MainWindow::updateFontSizeSetting(int size)
{
    QMdiSubWindow *sw = get_currentSubWindow();
    if ((sw != 0)||(sw != NULL)) {
        termsession *term = get_termsession(sw->windowTitle());
        //term->setScrollToBottom(!term->getScrollToBottom());
        settings->beginGroup(term->get_name());
        settings->setValue("fontSize", term->font().pointSize());
        settings->endGroup();
    }
}

void MainWindow::macroSetup()
{
    //TODO   macroSetup, script file setting....
    ui->actionMacroStart->setEnabled(true);
}

void MainWindow::macroStart()
{
    QMdiSubWindow *sw = get_currentSubWindow();
    if ((sw != 0)||(sw != NULL)) {
        termsession *term = get_termsession(sw->windowTitle());
        term->macroStart();
    }
}

void MainWindow::macroStop()
{
    QMdiSubWindow *sw = get_currentSubWindow();
    if ((sw != 0)||(sw != NULL)) {
        termsession *term = get_termsession(sw->windowTitle());
        term->macroStop();
    }
}
void MainWindow::updateActionMacroBtnStatus(Qt::HANDLE id)
{
    QMdiSubWindow *sw = get_currentSubWindow();
    if ((sw != 0)||(sw != NULL)) {
        termsession *term = get_termsession(sw->windowTitle());
        if (term->getMacroThreadId() == id) {
            updateActionMacroBtnStatus(term->isMacroRunning());
        }
    }
}
/*
 * when macro is running set bStatus = true
*/
void MainWindow::updateActionMacroBtnStatus(bool bStatus)
{
    ui->actionMacroSetup->setEnabled(!bStatus);
    ui->actionMacroStart->setEnabled(!bStatus);
    ui->actionMacroStop->setEnabled(bStatus);
}

