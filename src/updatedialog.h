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

namespace Ui {
class updatedialog;
}

class updatedialog : public QDialog
{
    Q_OBJECT

public:
    explicit updatedialog(QWidget *parent = 0);
    ~updatedialog();

private slots:
    void downloadFile();
    void httpFinished();
    void httpReadyRead();
    void networkReplyProgress(qint64 bytesRead, qint64 totalBytes);

    void getUpdate();
    void dlError(QNetworkReply::NetworkError err);
    void dlProgress(qint64 read, qint64 total);
    void dlFinished();

private:
    void startRequest(const QUrl &requestedUrl);
    QFile *openFileForWrite(const QString &fileName);
    bool fileExists(QString path);
    QJsonDocument loadJson(QString fileName);
    bool isLatestVersionExist(QString latestVersion);
    void setStatus(QString msg);

private:
    Ui::updatedialog *ui;
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

};

#endif // UPDATEDIALOG_H
