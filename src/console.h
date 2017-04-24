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

#ifndef CONSOLE_H
#define CONSOLE_H

#include <QPlainTextEdit>
#include <QMenu>
#include <QAction>

class Console : public QPlainTextEdit
{
    Q_OBJECT


public:
    explicit Console(QWidget *parent = 0);
    ~Console();

    void showDataOnTextEdit(const QByteArray &data);

    void setLocalEchoEnabled(bool set);
    void setScrollToBottom(bool set);
    bool getScrollToBottom();
    void setLogDatetime(bool set);
    void setTheme(QString sTheme);
    //void setMaximumBlockCount();
    void paste();

signals:
    void sig_DataReady(const QByteArray &data);
    Q_SIGNAL void fontSizeChanged(int size);

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    ///The user scrolled within console.
    void wheelEvent(QWheelEvent *event);
    //virtual void contextMenuEvent(QContextMenuEvent *e);

private slots:
    void showContextMenu(QPoint pt);
    void updateCopyAction(bool yes);


private:
    void createRightMenu();
    void createActions();
    void moveCurserToEnd();
    void saveCurser();
    void restoreCurser();
    QTextCursor getOldCurser();

private:
    bool localEchoEnabled;
    bool scrollToBottom;
    bool logDatetime;
    QMenu *rightMenu;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *clearAct;
    QAction *selectAllAct;
    int posCursor; //remember cursor pos variable
    QTextCursor prev_cursor;
};

#endif // CONSOLE_H
