/****************************************************************************
**
** Copyright (C) 2017 jimmy
**
****************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <QDialog>
#include <QDateTime>
#include <QDesktopServices>
#include <QNetworkAccessManager>
#include <QStandardPaths>
#include <QDir>
#if defined(Q_OS_WIN)
    #include <windef.h>
    #include <windows.h>
#endif
#include "const.h"

#include <QDebug>

//! [0]
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    detectSystem();
    //qDebug()<< QDateTime::currentDateTime()  << " account:" << m_OSInof.account;
    //setting
    settings = new QSettings();
    readPosSetting();
    loadOptions();
    settingDlg = new SettingsDialog();
    connect(settingDlg, SIGNAL(finished(int)), this, SLOT(slot_acceptSettingDlg(int)));
    optionDlg = new optionsDialog();
    connect(optionDlg, SIGNAL(finished(int)), this, SLOT(slot_acceptOptionDlg(int)));

    connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(slot_subWindowChanged(QMdiSubWindow*)));

    //this->updateActionBtnStatus(true);
    ui->actionQuit->setEnabled(true);
    initActionsConnections();
    initToolBar();

#ifndef QT_NO_SYSTEMTRAYICON
    createTrayActions();
    createTrayIcon();
    trayIcon->show();
#endif
    //get_session_num();
    //TODO: restore session/ start minial...

    if (CheckUpdate) {
        //TODO: should do it in background?, if there is update then show it!
        this->update(true);
    }

}


MainWindow::~MainWindow()
{
    delete settingDlg;
    delete ui;
    delete settings;
    delete optionDlg;
}

void MainWindow::receivedMessage( int instanceId, QByteArray message )
{
    qDebug() << "Received message from instance: " << instanceId;
    qDebug() << "Message Text: " << message;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {
        showTrayMessage("Notice", tr("The program will keep running in the "
                          "system tray. To terminate the program, "
                          "choose \"Quit\" in the context menu "
                          "of the system tray entry.") );
        hide();
        event->ignore();
    } else {
        //TODO: ask quit?
        savePosSetting();
        event->accept();
    }
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
        settings->setValue("theme", settingDlg->get_settings().theme);
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
            //still show close button in cascade/tile

            //subwin1->setWindowFlags(Qt::Window | Qt::WindowMinMaxButtonsHint);
            subwin1->setWidget(termSession);
            subwin1->setWindowIcon(QIcon(":/images/qtvt.png"));
            //subwin1->setAttribute(Qt::WA_DeleteOnClose, false);
            subwin1->resize(QSize(ui->mdiArea->width(),ui->mdiArea->height()));
            subwin1->setWindowTitle(sName);
            subwin1->systemMenu()->clear(); //remove mouse right click menu (not support close session tab, TODO)
            subwin1->systemMenu()->addAction(ui->actionClose_session); //QAction::eventFilter: Ambiguous shortcut overload: Ctrl+W
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
    //updateActionEditSessionBtnStatus(false);
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
        updateActionEditSessionBtnStatus(false);
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
void MainWindow::donate()
{
    //donate url
    QUrl myUrl(MYPAYPAL);
    QDesktopServices::openUrl(myUrl);
}

void MainWindow::update(bool showmode)
{
    updatedialog *updateDlg = new updatedialog(this, m_OSInof, AutoCloseUpdate);
    connect(updateDlg, SIGNAL(doExit()), this, SLOT(close()));
    connect(updateDlg, SIGNAL(doExec(QString)), this, SLOT(execFile(QString)));
    if (showmode) {
        updateDlg->show();
    } else {
        //updateDlg->setAutoClose(false);
        updateDlg->exec();
        delete updateDlg;
    }

}
void MainWindow::execFile(QString Filename)
{
    qDebug() <<"going to exec file: " << Filename ;
    QProcess *process = new QProcess(this);

    if (QSysInfo::productType() == "ubuntu") {
        QString file = "gdebi-gtk %1";
        process->startDetached(file.arg(Filename));
    } else if (QSysInfo::productType() == "windows") {
        process->startDetached(Filename);
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
    connect(ui->actionDonate, SIGNAL(triggered()), this, SLOT(donate()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    //other
    //history input
    connect(ui->HistoryEdit,SIGNAL(textChanged(QString)), this, SLOT(showInputHistory(QString)));
    connect(ui->HistoryEdit,SIGNAL(returnPressed()), ui->SendBtn, SLOT(click()));
    connect(ui->SendBtn, SIGNAL(pressed()), this, SLOT(sendSerialText()));
}

//load options from setting file to variable
void MainWindow::loadOptions()
{
    QSettings set;
    set.beginGroup("Main");
    StartOnBoot =set.value("StartOnBoot", true).toBool();
    StartMinimal =set.value("StartMinimal", false).toBool();
    RestortSession =set.value("RestortSession", true).toBool();
    CheckUpdate =set.value("CheckUpdate", true).toBool();
    AutoCloseUpdate = set.value("AutoCloseUpdate", false).toBool();
    set.endGroup();

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
            loadOptions();
            //TODO
            setAutoStart(StartOnBoot);
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
        settingDlg->setEditMode(true);
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

/*
 * bStatus: true : serial is connected/open
 *          false: serial is not connected/open
 */
void MainWindow::updateActionBtnStatus(bool bSerialConnected)
{
    //qDebug() << "updateActionBtnStatus: " <<bSerialConnected;
    ui->actionConnect->setEnabled(!bSerialConnected);
    ui->actionDisconnect->setEnabled(bSerialConnected);
    ui->actionScrollToBottom->setEnabled(bSerialConnected);
    //ui->actionConfigure->setEnabled(bStatus);
    //updateActionConfigureBtnStatus(bStatus);
}
/*
 * bStatus = False : no term session
 * bStatus = True : term session exist
 * */
void MainWindow::updateActionEditSessionBtnStatus(bool bTermSessionExist)
{
    //qDebug() << "updateActionEditSessionBtnStatus:" << bStatus;
    //current is connect
    ui->actionEdit_session->setEnabled(bTermSessionExist);
    ui->actionClose_session->setEnabled(bTermSessionExist);


    ui->actionLogFile->setEnabled(bTermSessionExist);
    ui->actionMacroSetup->setEnabled(!bTermSessionExist);
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
    }
    else
    {
        updateMenuSession(false);
        updateActionBtnStatus(false);
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
        //settings->setValue("fontSize", term->font().pointSize());
        settings->setValue("fontSize", size);
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
/*
 * detectSystem
 *  get current OS info/account
 */
void MainWindow::detectSystem()
{
    m_OSInof.cpuArch = QSysInfo::currentCpuArchitecture();
    m_OSInof.productType = QSysInfo::productType();
    m_OSInof.productVersion = QSysInfo::productVersion();
    getAccountName();
}
void MainWindow::getAccountName()
{
    #if defined(Q_OS_WIN)
        wchar_t acUserName[MAX_USERNAME];
        QString username="";
        DWORD nUserName = sizeof(acUserName);
        if (GetUserName(acUserName, &nUserName)) {
            m_OSInof.account = username.fromWCharArray(acUserName);
        }
    #elif defined(Q_OS_LINUX)
        //This will require some time (120ms?) to finish!!
        process=new QProcess();
        connect(process, SIGNAL(finished(int)), this, SLOT(setAccountName(int)));
        process->start("whoami");
    #else
        #warn("Not supported SYSTEM")
    #endif
}
void MainWindow::setAccountName(int code)
{
    m_OSInof.account =  process->readAllStandardOutput().replace("\n", "");
    //qDebug() << QDateTime::currentDateTime() <<" setAccountName:" << m_OSInof.account << "  size:"<< m_OSInof.account.length();
    Q_UNUSED(code);
}

void MainWindow::setAutoStart(bool start)
{

#if defined(Q_OS_WIN)
    QSettings regSettings(AUTOSTARTFOLDER, QSettings::NativeFormat);
#elif defined(Q_OS_LINUX)
    QString autodir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    autodir = autodir+ QDir::separator() +AUTOSTARTFOLDER;
    QString target = autodir + QDir::separator() + DESKTOPFILE;
#else
        #warn("Not supported SYSTEM")
#endif
    qDebug() << "StartOnBoot:" << StartOnBoot;
    if (start) {
        //setup auto start
#if defined(Q_OS_WIN)
        regSettings.setValue(APP_PRODUCT,
                           QCoreApplication::applicationFilePath().replace('/', '\\'));
#elif defined(Q_OS_LINUX)
        qDebug() << "target:"<< target;
        if (! QFile::exists(target))
        {
            qDebug() << "target" << target;
            if (!QFile::copy(INSTALLDESKTOPFILE, target))
            {
                QMessageBox::warning(this,tr("info"),
                                     tr("copy file from %s to %s fail").arg(INSTALLDESKTOPFILE).arg(target),
                                     QMessageBox::Ok);
            }
        }
#else
        #error("Not supported SYSTEM")
#endif

    } else {
        //remove auto start
#if defined(Q_OS_WIN)
        regSettings.remove(APP_PRODUCT);
#elif defined(Q_OS_LINUX)
        if (QFile::exists(target))
        {
            if (! QFile::remove(target)) {
                    QMessageBox::warning(this,tr("info"),
                                         tr("Delete file %s fail").arg(target),
                                         QMessageBox::Ok);
                }
        }
#else
    #error("Not supported SYSTEM")
#endif
    }
}

//systray
void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/images/qtvt.png"));
    trayIcon->setContextMenu(trayIconMenu);
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

}
void MainWindow::createTrayActions()
{
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}
void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        qDebug() << "TODO: QSystemTrayIcon::Trigger";
        break;
    case QSystemTrayIcon::DoubleClick:
        if (this->isHidden()){
            restoreAction->trigger();
        }
        break;
    case QSystemTrayIcon::MiddleClick:
        qDebug() << "TODO: QSystemTrayIcon::MiddleClick";
        break;
    default:
        ;
    }
}
void MainWindow::showTrayMessage(QString title, QString msg,
                                 QSystemTrayIcon::MessageIcon icon,
                                 int msecs)
{
    trayIcon->showMessage(title, msg, icon, msecs);
}
