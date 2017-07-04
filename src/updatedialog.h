#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QDialog>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QFileInfo>
#include <QProgressDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>

#include "const.h"

namespace Ui {
class updatedialog;
}

class updatedialog : public QDialog
{
    Q_OBJECT

public:
    explicit updatedialog(QWidget *parent ,
                          OSInfo osinfo, bool autoclose=false );
    ~updatedialog();
    void setAutoClose(bool close);

signals:
    void doExit();
    void doExec(QString filename);

private:
    Q_SLOT void downloadFile();
    Q_SLOT void httpFinished();
    Q_SLOT void httpReadyRead();
    Q_SLOT void networkReplyProgress(qint64 bytesRead, qint64 totalBytes);

    Q_SLOT void getUpdate();
    Q_SLOT void dlError(QNetworkReply::NetworkError err);
    Q_SLOT void dlProgress(qint64 read, qint64 total);
    Q_SLOT void dlFinished();
    Q_SLOT void dlReadyRead();
    Q_SLOT void timerHandler();
    Q_SLOT void setAutoCloseTimer(int state);

    void startRequest(const QUrl &requestedUrl);
    QFile *openFileForWrite(const QString &fileName);
    bool fileExists(QString path);
    QJsonDocument loadJson(QString fileName);
    bool isLatestVersionExist(QString latestVersion);
    int cmpVersion(const char *v1, const char *v2);
    void setStatus(QString msg);
    void setCloseCaption(QString title);


    Ui::updatedialog *ui;
    QString cpuArch;
    QString productType;
    QUrl url;
    QString downloadDirectory;
    QNetworkAccessManager qnam;
    QNetworkReply *reply;
    QFile *file;
    bool httpRequestAborted;
    QString latestDLFilename;
    QUrl latestDLUrl;
    qint64 latestDLSize;
    QFile *dlfile;

    bool m_autoclose;
    QTimer *autoclosetimer;
    int autoclosecount;
};

#endif // UPDATEDIALOG_H
