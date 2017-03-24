VERSION = 2017.3.24.0
QMAKE_TARGET_COMPANY = coolshou
QMAKE_TARGET_PRODUCT = qtvt
QMAKE_TARGET_DESCRIPTION = qt base serial virtual console
QMAKE_TARGET_COPYRIGHT = copyright 2017


greaterThan(QT_MAJOR_VERSION, 4) {
    QT       += widgets serialport
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
    src/termsession.cpp

HEADERS += \
    src/console.h \
    src/mainwindow.h \
    src/settingsdialog.h \
    src/termsession.h \
    src/const.h

FORMS += \
    src/mainwindow.ui \
    src/settingsdialog.ui

RESOURCES += \
    qtvt.qrc

DISTFILES += \
    README.md

DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += APP_COMPANY=\\\"$$QMAKE_TARGET_COMPANY\\\"
DEFINES += APP_PRODUCT=\\\"$$QMAKE_TARGET_PRODUCT\\\"
