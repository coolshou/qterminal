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

#include <QDebug>

Console::Console(QWidget *parent)
    : QPlainTextEdit(parent)
    , localEchoEnabled(false), scrollToBottom(true)
{
    //TODO: Maximum Block Count
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
    copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
    connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));
    updateCopyAction(false);

    pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
    connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()));

    clearAct = new QAction(QIcon(":/images/clear.png"), tr("Clear"), this);
    connect(clearAct, SIGNAL(triggered()), this, SLOT(clear()));

    selectAllAct = new QAction(QIcon(":/images/selectAll.png"), tr("&Select All"), this);
    connect(selectAllAct, SIGNAL(triggered()), this, SLOT(selectAll()));
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
    //TODO: custom menu
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
    rightMenu->addSeparator();
    rightMenu->addAction(tr("TODO: other menu"));

}
void Console::showContextMenu(QPoint pt)
{
    qDebug() << "showContextMenu: " << pt;
    if(rightMenu)
        rightMenu->exec(this->mapToGlobal(pt));
}

void Console::putData(const QByteArray &data)
{
    insertPlainText(QString(data));

    if (scrollToBottom) {
        QScrollBar *bar = verticalScrollBar();
        bar->setValue(bar->maximum());
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
void Console::setTheme(QString sTheme)
{
    QPalette p = palette();
    //TODO: color theme
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::green);
    setPalette(p);

}

void Console::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Backspace:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
        break;
    default:
        if (localEchoEnabled)
            QPlainTextEdit::keyPressEvent(e);
        emit getData(e->text().toLocal8Bit());
    }
}

void Console::mousePressEvent(QMouseEvent *e)
{
    qDebug() << "mousePressEvent: " << e->pos();
    Q_UNUSED(e)
    setFocus();
}

void Console::mouseDoubleClickEvent(QMouseEvent *e)
{

    qDebug() << "mouseDoubleClickEvent: " << e->pos();
    //TODO: not work
    //QTextCursor tc(this->document());
    //tc.select(QTextCursor::WordUnderCursor);
    //qDebug() << "pos:" << this->textCursor().position();
    //this->textCursor().select(QTextCursor::WordUnderCursor);

    Q_UNUSED(e)
}

//replace by showContextMenu
void Console::contextMenuEvent(QContextMenuEvent *e)
{
    qDebug() << "contextMenuEvent: " << e->pos();
    if(rightMenu)
        rightMenu->exec(e->globalPos());
//    Q_UNUSED(e)
}
