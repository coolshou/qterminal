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

#include "console.h"

#include <QScrollBar>
#include <QTextCursor>
#include <QDebug>

#include <QClipboard>
#include <QGuiApplication>

Console::Console(QWidget *parent)
    : QPlainTextEdit(parent)
    , localEchoEnabled(false), scrollToBottom(false)
{
    this->setLineWrapMode(LineWrapMode::NoWrap);
    //Maximum Block Count
    document()->setMaximumBlockCount(2000);
    setTheme("default");
    createActions();
    createRightMenu();
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,SIGNAL(customContextMenuRequested(QPoint)), this,SLOT(showContextMenu(QPoint)));
    connect(this, SIGNAL(copyAvailable(bool)), this, SLOT(updateCopyAction(bool)));

}
Console::~Console()
{
    delete rightMenu;
}

void Console::createActions()
{
    copyAct = new QAction(QIcon(":/images/copy.png"), tr("Copy"), this);
    //ctrl +c will send ^c in serial
    //copyAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));
    updateCopyAction(false);

    pasteAct = new QAction(QIcon(":/images/paste.png"), tr("Paste"), this);
    //pasteAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V));
    connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()));

    clearAct = new QAction(QIcon(":/images/clear.png"), tr("Clear"), this);
    //TODO: clearAct->setShortcut
    //clearAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));
    connect(clearAct, SIGNAL(triggered()), this, SLOT(clear()));

    selectAllAct = new QAction(QIcon(":/images/selectAll.png"), tr("Select All"), this);
    //TODO: selectAllAct->setShortcut
    //selectAllAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));
    connect(selectAllAct, SIGNAL(triggered()), this, SLOT(selectAll()));
}
void Console::paste()
{
    this->moveCurserToEnd();
    QClipboard *p_Clipboard =  QGuiApplication::clipboard();
    //qDebug() << "doPaste: " << p_Clipboard->text(QClipboard::Clipboard);
    //this->paste(); //this will not send text to serial!!
    emit sig_DataReady(p_Clipboard->text(QClipboard::Clipboard).toLocal8Bit());
}

void Console::updateCopyAction(bool yes)
{
    if (copyAct)
    {
        copyAct->setEnabled(yes);
    }
}

void Console::createRightMenu()
{
    //custom right click menu
    rightMenu = createStandardContextMenu();
    /* StandardContextMenu
     * QAction(0x132e2f0, name = "edit-undo"),
     * QAction(0x1353d40, name = "edit-redo"),
     * QAction(0x1369870),
     * QAction(0x1369240, name = "edit-cut"),
     * QAction(0x132e2d0, name = "edit-copy"),
     * QAction(0x134a700, name = "edit-paste"),
     * QAction(0x13497e0, name = "edit-delete"),
     * QAction(0x13526b0),
     * QAction(0x1343ca0, name = "select-all")
     * */
    rightMenu->clear(); //remove default menu item
    rightMenu->addAction(selectAllAct);
    rightMenu->addAction(copyAct);
    rightMenu->addAction(pasteAct);
    rightMenu->addSeparator();
    rightMenu->addAction(clearAct);
    //rightMenu->addSeparator();
    //rightMenu->addAction(tr("TODO: other menu"));

}
void Console::showContextMenu(QPoint pt)
{
    if(rightMenu)
        rightMenu->exec(this->mapToGlobal(pt));
}

void Console::showDataOnTextEdit(const QByteArray &data)
{
    if (!scrollToBottom) {
        saveCurser();
    }
    moveCurserToEnd();
    //TODO: not show special key such as backspace...
    insertPlainText(QString(data));
    if (!scrollToBottom) {
        restoreCurser();
    } else {
        QScrollBar *bar = verticalScrollBar();
        bar->setValue(bar->maximum());
        this->moveCurserToEnd();
    }
}

void Console::setLocalEchoEnabled(bool set)
{
    localEchoEnabled = set;
}
void Console::setScrollToBottom(bool set)
{
    scrollToBottom = set;
}
bool Console::getScrollToBottom()
{
    return scrollToBottom;
}

void Console::setTheme(QString sTheme)
{
    QPalette p = palette();
    //TODO: color theme
    Q_UNUSED(sTheme)
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::green);
    QFont f;
    //f.setFamily(mSetting->value("fontFamily").toString());
    f.setPointSize(10);
    //f.setBold(true);
    this->setFont(f);
    setPalette(p);

}

void Console::moveCurserToEnd()
{
    moveCursor(QTextCursor::End);
}
void Console::saveCurser()
{
    prev_cursor = this->textCursor();
}
QTextCursor Console::getOldCurser()
{
    return prev_cursor;
}
void Console::restoreCurser()
{
    setTextCursor(prev_cursor);
}

void Console::keyPressEvent(QKeyEvent *e)
{
    //qDebug() << "keyPressEvent:" << e->type();
    if(e->type() == QKeyEvent::KeyPress) {
        //when we press any key do scroll to bottom
        this->moveCurserToEnd();
        QByteArray test;
        QByteArray ba;
        /*
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(e);
        if (keyEvent->key() == Qt::Key_A) {
            qDebug() << "CTRL-A Key pressed" << keyEvent->key();
            return true;
        }*/
        //TODO: shift + insert = paste clipboard data
        //(if selected text, will copy selected text and paste)
        //
        if(e->matches(QKeySequence::Copy)) {
            //send CTRL+C";
            QString test="\x03";
            emit sig_DataReady(test.toLocal8Bit());
        } else {
            //qDebug() << "key:" << e->key();
            switch (e->key()) {
            //TODO: backspace, delete
            /*
            case Qt::Key_Return:
                qDebug() << "key Return";
                ba.resize(2);
                ba[0] = 0x0d; //CR (Carriage Return) \r 13=0x0d
                ba[1] = 0x0a; //LF (Line Feed, or new line). \n 10=0x0a
                emit sig_DataReady(ba);
                break;
                */
            case Qt::Key_Backspace:
                //test="\x08"; //Send an ASCII backspace character (0x08).
                ba.resize(1);
                ba[0] = 0x08;
                qDebug() <<  "Key Backspace: " << ba;
                emit sig_DataReady(ba);//TODO: not update to console?
                break;
            case Qt::Key_Delete:
                //test="\x7F"; //Send an ASCII delete character (0x7F).
                //emit sig_DataReady(test.toLocal8Bit());
                break;
            /*
             * http://ascii-table.com/ansi-escape-sequences.php
             *
             *TODO: Up arrow         E0 48
             *TODO: Down arrow       E0 50
             *TODO: Left arrow       E0 4B
             *TODO: Right arrow      E0 4D

             * Up key - 224 72
             * Down key - 224 80
             * Left key - 224 75
             * Right key - 224 77
             Cursor Up:       escape-[-A => \x1B\x5B\x61
             Cursor Down:     escape-[-B => \x1B\x5B\x62
             Cursor Forward:  escape-[-C => \x1B\x5B\x63
             Cursor Backward: escape-[-D => \x1B\x5B\x64
            */
            case Qt::Key_Up: // TODO: history Key_Up
                test="\x1B\x5B\x61";
                //char arr[3] = {0x1B, 0x5B, 0x41};
                ba.resize(3);
                ba[0] = 0x1B;
                ba[1] = 0x5B;
                ba[2] = 0x41;
                qDebug() <<  "key up: " << ba;
                emit sig_DataReady(ba);
                break;
            case Qt::Key_Down: // TODO: history Key_Down
                //test="\xE0\x50";
                //emit sig_DataReady(test.toLocal8Bit());
                break;
            case Qt::Key_Left://TODO: Qt::Key_Left
                //test="\xE0\x4B";
                //emit sig_DataReady(test.toLocal8Bit());
                break;
            case Qt::Key_Right://TODO: Qt::Key_Right
                //test="\xE0\x4D";
                //emit sig_DataReady(test.toLocal8Bit());
                break;
            default:
                if (localEchoEnabled)
                    QPlainTextEdit::keyPressEvent(e);
                emit sig_DataReady(e->text().toLocal8Bit());
            }
        }
    }
}

void Console::mousePressEvent(QMouseEvent *e)
{
    //QTextEdit::mousePressEvent(e);
    if (e->buttons() & Qt::LeftButton)
    {
        QTextCursor cursor = cursorForPosition(e->pos());
        posCursor = cursor.position();
        cursor.clearSelection();
        setTextCursor(cursor);
    }
    //setFocus();
    if (e->buttons() & Qt::RightButton)
    {
        qDebug() << "Qt::RightButton";
    }
    if (e->buttons() & Qt::MiddleButton)
    {
        //when some text selected, copy first
        QString sel = this->textCursor().selectedText();
        if (!sel.isEmpty()) {
            QGuiApplication::clipboard()->setText(sel);
        }
        paste();
    }
    QPlainTextEdit::mousePressEvent(e);
}

void Console::mouseReleaseEvent(QMouseEvent *e)
{
    //with following line, on mouse middle button release will cause paste text to Console. why?
    //QPlainTextEdit::mousePressEvent(e);
    Q_UNUSED(e);
}

void Console::mouseMoveEvent(QMouseEvent *e)
{
    //QTextEdit::mouseMoveEvent(e);
    if (e->buttons() & Qt::LeftButton)
    {
        QTextCursor cursor = textCursor();
        QTextCursor endCursor = cursorForPosition(e->pos()); // key point
        cursor.setPosition(posCursor, QTextCursor::MoveAnchor);
        cursor.setPosition(endCursor.position(), QTextCursor::KeepAnchor);
        setTextCursor(cursor);
    }
}
void Console::mouseDoubleClickEvent(QMouseEvent *e)
{
    qDebug() << "mouseDoubleClickEvent: " << e->pos();
    if (e->buttons() & Qt::LeftButton)
    {
        QTextCursor tc = this->textCursor();
        tc.select(QTextCursor::WordUnderCursor);
        // where the selction starts
        int newpos = tc.selectionStart();
        // where the selction end
        int endpos = tc.selectionEnd();
        // select from newpos to endpos (order is important)
        tc.setPosition(newpos, QTextCursor::MoveAnchor);
        tc.setPosition(endpos, QTextCursor::KeepAnchor);
        setTextCursor(tc);//mark it selected
//        QString SearchedText = tc.selectedText();
//        qDebug()<< "oldpos:" << oldpos << " newpos:" << newpos << ", " << SearchedText ;
    }
}

/**
 * Use Ctrl + mouse wheel to increase/decrease font size in consoles.
 *
 * @param event
 *      The wheel event.
 */
void Console::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier)
    {
        QWheelEvent *wheel = static_cast<QWheelEvent*>(event);
        if(wheel->delta() > 0)
        {
            //TODO: font size
            //this->font().pointSize()
            zoomIn();
        } else {
            zoomOut();
        }
        //qDebug() << "font pointsize: " << this->font().pointSize();
        emit fontSizeChanged(this->font().pointSize());
    }

    //Forward event to parent for normal scrolling.
    QPlainTextEdit::wheelEvent(event);
}
//replace by showContextMenu
/*
void Console::contextMenuEvent(QContextMenuEvent *e)
{
    qDebug() << "contextMenuEvent: " << e->pos();
    if(rightMenu)
        rightMenu->exec(e->globalPos());
//    Q_UNUSED(e)
}
*/
