/****************************************************************************
**
** Copyright (C) 2017 jimmy
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
    Q_SLOT void paste();

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
