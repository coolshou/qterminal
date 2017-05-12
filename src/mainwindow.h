/****************************************************************************
**
** Copyright (C) 2017 jimmy
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
#include <QSystemTrayIcon>

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
    Q_SLOT void update(bool showmode=false);
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
    //systray
    void createTrayIcon();
    void createTrayActions();
    Q_SLOT void iconActivated(QSystemTrayIcon::ActivationReason reason);

    Ui::MainWindow *ui;
    QSettings *settings;
    SettingsDialog *settingDlg;
    optionsDialog *optionDlg ;
    QList<termsession *> sessionlist;
    //updatedialog *updateDlg;
    OSInfo m_OSInof;
    QProcess *process;
    //systray
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;
    //options
    bool StartOnBoot;
    bool StartMinimal;
    bool RestortSession;
    bool CheckUpdate;
    bool AutoCloseUpdate;
};

#endif // MAINWINDOW_H
