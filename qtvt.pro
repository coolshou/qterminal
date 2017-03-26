VERSION = 2017.3.27.0
QMAKE_TARGET_COMPANY = coolshou
QMAKE_TARGET_PRODUCT = qtvt
QMAKE_TARGET_DESCRIPTION = qt base serial virtual console
QMAKE_TARGET_COPYRIGHT = copyright 2017


greaterThan(QT_MAJOR_VERSION, 4) {
    QT       += widgets serialport network
} else {
    include($$QTSERIALPORT_PROJECT_ROOT/src/serialport/qt4support/serialport.prf)
}

CFLAG+=-fPIC

TARGET = qtvt
TEMPLATE = app

SOURCES += \
    src/console.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/settingsdialog.cpp \
    src/termsession.cpp \
    src/updatedialog.cpp

HEADERS += \
    src/console.h \
    src/mainwindow.h \
    src/settingsdialog.h \
    src/termsession.h \
    src/const.h \
    src/updatedialog.h

FORMS += \
    src/mainwindow.ui \
    src/settingsdialog.ui \
    src/updatedialog.ui

RESOURCES += \
    qtvt.qrc

DISTFILES += \
    README.md \
    debian/changelog \
    qtvt.desktop

DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += APP_COMPANY=\\\"$$QMAKE_TARGET_COMPANY\\\"
DEFINES += APP_PRODUCT=\\\"$$QMAKE_TARGET_PRODUCT\\\"
GIT_VERSION = $$system(git --git-dir $$PWD/.git --work-tree $$PWD describe --always --tags)
DEFINES += GIT_VERSION=\\\"$$GIT_VERSION\\\"
