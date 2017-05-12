/****************************************************************************
**
** Copyright (C) 2017 jimmy
**
****************************************************************************/

#include <QApplication>
#include <QCoreApplication>
#include <QObject>
#include <SingleApplication/singleapplication.h>
#include <QMessageBox>

#include "mainwindow.h"
#include "const.h"

int main(int argc, char *argv[])
{
    //QApplication app(argc, argv);
    SingleApplication app( argc, argv );
#ifndef QT_NO_SYSTEMTRAYICON
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(0, QObject::tr("Systray"),
                              QObject::tr("I couldn't detect any system tray "
                                          "on this system."));
        return 1;
    }
    SingleApplication::setQuitOnLastWindowClosed(false);
#else
    #warn("QSystemTrayIcon is not supported on this platform")
#endif
    app.setOrganizationName(APP_COMPANY);
    app.setOrganizationDomain(MYORGDOMAIN);
    app.setApplicationName(APP_PRODUCT);
    MainWindow w;
    if (app.isSecondary())
    {
        app.sendMessage(app.arguments().join(' ').toUtf8());
        app.exit(0);
    } else {
        //primary instant
        QObject::connect(&app, &SingleApplication::receivedMessage,
                        &w,
                        &MainWindow::receivedMessage );
        //raise window
        QObject::connect(&app, &SingleApplication::instanceStarted,
                        [ &w ]() {
                            w.raise();
                            w.activateWindow();
                        });
    }
    w.show();
    return app.exec();
}
