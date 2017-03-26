#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QDialog>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QFileInfo>
#include <QProgressDialog>

class ProgressDialog : public QProgressDialog {
    Q_OBJECT

public:
    explicit ProgressDialog(const QUrl &url, QWidget *parent = Q_NULLPTR);

public slots:
   void networkReplyProgress(qint64 bytesRead, qint64 totalBytes);
};

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
    void cancelDownload();
    void httpReadyRead();
    void networkReplyProgress(qint64 bytesRead, qint64 totalBytes);

private:
    void startRequest(const QUrl &requestedUrl);
    QFile *openFileForWrite(const QString &fileName);

private:

    Ui::updatedialog *ui;
    QUrl url;
    QNetworkAccessManager qnam;
    QNetworkReply *reply;
    QFile *file;
    bool httpRequestAborted;
};

#endif // UPDATEDIALOG_H
