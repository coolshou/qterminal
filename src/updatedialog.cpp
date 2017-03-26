#include "updatedialog.h"
#include "ui_updatedialog.h"

//#include <QProgressDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>

#include "const.h"

#include <QDebug>

static const char defaultFileName[] = "release.html";

//=================================================================
ProgressDialog::ProgressDialog(const QUrl &url, QWidget *parent)
  : QProgressDialog(parent)
{
    setWindowTitle(tr("Download Progress"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setLabelText(tr("Downloading %1.").arg(url.toDisplayString()));
    setMinimum(0);
    setValue(0);
    setMinimumDuration(0);
}

void ProgressDialog::networkReplyProgress(qint64 bytesRead, qint64 totalBytes)
{
    setMaximum(totalBytes);
    setValue(bytesRead);
}
//=================================================================

updatedialog::updatedialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::updatedialog)
{
    ui->setupUi(this);
    //tmp download Directory
    QString downloadDirectory = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    if (downloadDirectory.isEmpty() || !QFileInfo(downloadDirectory).isDir())
        downloadDirectory = QDir::currentPath();
    qDebug() << "download Directory:" << downloadDirectory;

    downloadFile();
}

updatedialog::~updatedialog()
{
    delete ui;
}

void updatedialog::downloadFile()
{
//    const QString urlSpec = urlLineEdit->text().trimmed();
//    if (urlSpec.isEmpty())
//        return;

    const QUrl newUrl = QUrl::fromUserInput(MYRELEASEURL);
    if (!newUrl.isValid()) {
        QMessageBox::information(this, tr("Error"),
                                 tr("Invalid URL: %1: %2").arg(MYRELEASEURL, newUrl.errorString()));
        return;
    }

    QString fileName = newUrl.fileName();
//    if (fileName.isEmpty())
//        fileName = defaultFileLineEdit->text().trimmed();
    if (fileName.isEmpty())
        fileName = defaultFileName;
//    QString downloadDirectory = QDir::cleanPath(downloadDirectoryLineEdit->text().trimmed());
//    if (!downloadDirectory.isEmpty() && QFileInfo(downloadDirectory).isDir())
//        fileName.prepend(downloadDirectory + '/');
//    if (QFile::exists(fileName)) {
//        if (QMessageBox::question(this, tr("Overwrite Existing File"),
//                                  tr("There already exists a file called %1 in "
//                                     "the current directory. Overwrite?").arg(fileName),
//                                  QMessageBox::Yes|QMessageBox::No, QMessageBox::No)
//            == QMessageBox::No)
//            return;
//        QFile::remove(fileName);
//    }

    file = openFileForWrite(fileName);
    if (!file)
        return;

//    downloadButton->setEnabled(false);

    // schedule the request
    startRequest(newUrl);
}

void updatedialog::startRequest(const QUrl &requestedUrl)
{
    url = requestedUrl;
    httpRequestAborted = false;

    QNetworkRequest request(url);
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());

    reply = qnam.get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
    connect(reply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));

    connect(reply, SIGNAL(bytesWritten(qint64)), this, SLOT(networkReplyProgress(int)));
/*
    ProgressDialog *progressDialog = new ProgressDialog(url, this);
    progressDialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(progressDialog, SIGNAL(canceled()), this, SLOT(cancelDownload()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), progressDialog, SLOT(networkReplyProgress(qint64,qint64)));
    connect(reply, SIGNAL(finished()), progressDialog, SLOT(hide()));
    progressDialog->show();
*/
    ui->statusLabel->setText(tr("Downloading %1...").arg(url.toString()));
}
void updatedialog::httpFinished()
{
    qDebug() << "httpFinished:";

    QFileInfo fi;
    if (file) {
        fi.setFile(file->fileName());
        qDebug() << "fileName:" << file->fileName();
        qDebug() << file->readAll();
        file->close();

        delete file;
        file = Q_NULLPTR;
    }

    if (httpRequestAborted) {
        reply->deleteLater();
        reply = Q_NULLPTR;
        return;
    }

    if (reply->error()) {
        QFile::remove(fi.absoluteFilePath());
        ui->statusLabel->setText(tr("Download failed:\n%1.").arg(reply->errorString()));
        ui->downloadButton->setEnabled(true);
        reply->deleteLater();
        reply = Q_NULLPTR;
        return;
    }

    const QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    reply->deleteLater();
    reply = Q_NULLPTR;

    if (!redirectionTarget.isNull()) {
        const QUrl redirectedUrl = url.resolved(redirectionTarget.toUrl());
        if (QMessageBox::question(this, tr("Redirect"),
                                  tr("Redirect to %1 ?").arg(redirectedUrl.toString()),
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
            ui->downloadButton->setEnabled(true);
            return;
        }
        file = openFileForWrite(fi.absoluteFilePath());
        if (!file) {
            ui->downloadButton->setEnabled(true);
            return;
        }
        startRequest(redirectedUrl);
        return;
    }

    ui->statusLabel->setText(tr("Downloaded %1 bytes to %2\nin\n%3")
                         .arg(fi.size()).arg(fi.fileName(), QDir::toNativeSeparators(fi.absolutePath())));
//    if (launchCheckBox->isChecked())
//        QDesktopServices::openUrl(QUrl::fromLocalFile(fi.absoluteFilePath()));
    ui->downloadButton->setEnabled(true);
}
void updatedialog::httpReadyRead()
{
    // this slot gets called every time the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    if (file) {
        QByteArray a = reply->readAll();
        //qDebug() << "data: " << a;
        file->write(a);
    }
}
void updatedialog::cancelDownload()
{
    ui->statusLabel->setText(tr("Download canceled."));
    httpRequestAborted = true;
    reply->abort();
    ui->downloadButton->setEnabled(true);
}
QFile *updatedialog::openFileForWrite(const QString &fileName)
{
    QScopedPointer<QFile> file(new QFile(fileName));
    if (!file->open(QIODevice::ReadWrite)) {
    //if (!file->open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("Error"),
                                 tr("Unable to save the file %1: %2.")
                                 .arg(QDir::toNativeSeparators(fileName),
                                      file->errorString()));
        return Q_NULLPTR;
    }
    return file.take();
}
void updatedialog::networkReplyProgress(qint64 bytesRead, qint64 totalBytes)
{
    ui->progressBar->setMaximum(totalBytes);
    ui->progressBar->setValue(bytesRead);
}
