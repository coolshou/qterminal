VERSION = 2017.5.14.1
QMAKE_TARGET_COMPANY = coolshou
QMAKE_TARGET_PRODUCT = qtvt
QMAKE_TARGET_DESCRIPTION = qt base serial virtual console
QMAKE_TARGET_COPYRIGHT = copyright 2017


greaterThan(QT_MAJOR_VERSION, 4) {
    QT       += widgets serialport network
} else {
    include($$QTSERIALPORT_PROJECT_ROOT/src/serialport/qt4support/serialport.prf)
}
QT += script

include(SingleApplication/singleapplication.pri)
DEFINES += QAPPLICATION_CLASS=QApplication

CONFIG += c++11

CFLAG+=-fPIC

TARGET = qtvt
TEMPLATE = app

SOURCES += \
    src/console.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/settingsdialog.cpp \
    src/termsession.cpp \
    src/updatedialog.cpp \
    src/optionsdialog.cpp \
    src/script/scriptEngine.cpp \
    src/script/scriptThread.cpp \
    src/macro/macroThread.cpp \
    src/macro/macroWorker.cpp \
    src/consoletheme.cpp

HEADERS += \
    src/console.h \
    src/mainwindow.h \
    src/settingsdialog.h \
    src/termsession.h \
    src/const.h \
    src/updatedialog.h \
    src/optionsdialog.h \
    src/script/scriptEngine.h \
    src/script/scriptThread.h \
    src/macro/macroThread.h \
    src/macro/macroWorker.h \
    src/consoletheme.h

FORMS += \
    src/mainwindow.ui \
    src/settingsdialog.ui \
    src/updatedialog.ui \
    src/optionsdialog.ui

RESOURCES += \
    qtvt.qrc

DISTFILES += \
    README.md \
    debian/changelog \
    qtvt.desktop \
    debian/compat \
    debian/control \
    debian/copyright \
    debian/rules \
    debian/source/format \
    TODO.txt \
    installer/qtvt.nsi

win32: {
#win32-msvc* {
#    MSVC_VER = $$(VisualStudioVersion)
#    equals(MSVC_VER, 14.0){
#        message("msvc 2015")
#    }
#}
#equals(QMAKE_TARGET.arch, x86) {
#    message("x86")
#}

QMAKE_CFLAGS+="-source-charset:utf-8"

RC_ICONS += images/qtvt.ico

#*-64 {
#    message( "Building for 64 bit")
#}

release: DESTDIR = build/$$QMAKE_TARGET.arch/release
debug:   DESTDIR = build/$$QMAKE_TARGET.arch/debug

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui

}

DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += APP_COMPANY=\\\"$$QMAKE_TARGET_COMPANY\\\"
DEFINES += APP_PRODUCT=\\\"$$QMAKE_TARGET_PRODUCT\\\"
GIT_VERSION = $$system(git --git-dir $$PWD/.git --work-tree $$PWD describe --always)
DEFINES += GIT_VERSION=\\\"$$GIT_VERSION\\\"
