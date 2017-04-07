#include "updatedialog.h"
#include "ui_updatedialog.h"

#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>

#include "const.h"

#include <QDebug>

static const char defaultFileName[] = "qtvt_release.html";

updatedialog::updatedialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::updatedialog)
{
    ui->setupUi(this);
    ui->currentVersion->setText(APP_VERSION);
    ui->latestVersion->setText("checking");
    connect(ui->updateButton, SIGNAL(pressed()), this, SLOT(getUpdate()));

    //tmp download Directory
    QString downloadDirectory = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    if (downloadDirectory.isEmpty() || !QFileInfo(downloadDirectory).isDir())
        downloadDirectory = QDir::currentPath();

    downloadFile();
}

updatedialog::~updatedialog()
{
    delete ui;
}

void updatedialog::downloadFile()
{
    const QUrl newUrl = QUrl::fromUserInput(MYRELEASEURL);
    if (!newUrl.isValid()) {
        QMessageBox::information(this, tr("Error"),
                                 tr("Invalid URL: %1: %2").arg(MYRELEASEURL, newUrl.errorString()));
        return;
    }

    QString fileName = newUrl.fileName();
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
    connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(networkReplyProgress(qint64, qint64)));

    setStatus(tr("Downloading %1...").arg(url.toString()));
}
void updatedialog::httpFinished()
{
    ui->progressBar->setValue(0);

    QFileInfo fi;
    if (file) {
        fi.setFile(file->fileName());
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
        setStatus(tr("Error! not found latest version."));
        networkReplyProgress(0,100);
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
            return;
        }
        file = openFileForWrite(fi.absoluteFilePath());
        if (!file) {
            return;
        }
        startRequest(redirectedUrl);
        return;
    }

    //parser JSON file
    QJsonDocument jsonDoc = loadJson(fi.absoluteFilePath());
    //QString strJson(jsonDoc.toJson(QJsonDocument::Compact));
    //qDebug()<<strJson.toStdString().data();
    QJsonObject json_obj=jsonDoc.object();
    QString name=json_obj["name"].toString();
    ui->latestVersion->setText(name);
    if (isLatestVersionExist(name)) {
        //TODO: Latest file to download and install
        //assets/name, browser_download_url, size
        QJsonArray assets_arr=json_obj["assets"].toArray();
        for(int i=0; i<assets_arr.size(); i++){
            QJsonObject assets_obj=assets_arr[i].toObject();
            //TODO: check name-> platform file (deb, amd64 ...)
            qDebug() <<"assets name:" << assets_obj["name"].toString();
            qDebug() <<"assets browser_download_url:" << assets_obj["browser_download_url"].toString();
            qDebug() <<"assets size:" << assets_obj["size"].toInt();
        }
        setStatus(tr("Found New version!"));
        ui->updateButton->setEnabled(true);
    } else {
        setStatus(tr("No new version founded!"));
    }

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

QFile *updatedialog::openFileForWrite(const QString &fileName)
{
    if (fileExists(fileName)) {
        QFile::remove(fileName);
    }

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
bool updatedialog::fileExists(QString path) {
    QFileInfo check_file(path);
    // check if file exists and if yes: Is it really a file and no directory?
    return check_file.exists() && check_file.isFile();
}
QJsonDocument updatedialog::loadJson(QString fileName)
{
    QFile jsonFile(fileName);
    jsonFile.open(QFile::ReadOnly);
    return QJsonDocument().fromJson(jsonFile.readAll());
}

bool updatedialog::isLatestVersionExist(QString latestVersion)
{
    int iLatest = latestVersion.replace("v","").replace(".", "").toInt();
    QString ver = APP_VERSION;
    int iVer = ver.replace(".","").toInt();
    if (iLatest > iVer) {
        return true;
    } else {
        return false;
    }
}
void updatedialog::setStatus(QString msg)
{
    ui->statusLabel->setText(msg);
}
void updatedialog::getUpdate()
{
    qDebug() << "TODO: getUpdate";
}
