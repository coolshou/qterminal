greaterThan(QT_MAJOR_VERSION, 4) {
    QT       += widgets serialport
} else {
    include($$QTSERIALPORT_PROJECT_ROOT/src/serialport/qt4support/serialport.prf)
}

CFLAG+=-fPIC

TARGET = qtterminal
TEMPLATE = app

SOURCES += \
    src/console.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/settingsdialog.cpp

HEADERS += \
    src/console.h \
    src/mainwindow.h \
    src/settingsdialog.h

FORMS += \
    src/mainwindow.ui \
    src/settingsdialog.ui

RESOURCES += \
    qtterminal.qrc
