#include <QNetworkReply>
#include <QImage>
#include <QFileInfo>

#include "filedownloader.h"
#include <imageitem.h>

// b236a7a2955f4563bd77338a20437e53

DownloadManager::DownloadManager()
{
    connect(&m_manager, SIGNAL(finished(QNetworkReply*)),
            SLOT(downloadFinished(QNetworkReply*)));
}

void DownloadManager::downloadJSON(const QUrl &url)
{
    QNetworkRequest request(url);
    QNetworkReply *reply = m_manager.get(request);

#ifndef QT_NO_SSL
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), SLOT(sslErrors(QList<QSslError>)));
#endif
    m_currentDownloads.append(reply);
}

void DownloadManager::downloadImage(const ImageItem item)
{
    QNetworkRequest request(item.url());
    QNetworkReply *reply = m_manager.get(request);

#ifndef QT_NO_SSL
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), SLOT(sslErrors(QList<QSslError>)));
#endif
    m_pendingImages.insert(item.url(), item);
}

QString DownloadManager::saveFileName(const QUrl &url)
{
    QString path = url.path();
    QString basename = QFileInfo(path).fileName();

    if (basename.isEmpty())
        basename = "download";

    if (QFile::exists(basename)) {
        // already exists, don't overwrite
        int i = 0;
        basename += '.';
        while (QFile::exists(basename + QString::number(i)))
            ++i;

        basename += QString::number(i);
    }

    return basename;
}

bool DownloadManager::saveToDisk(const QString &filename, QIODevice *data)
{
    /*
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        fprintf(stderr, "Could not open %s for writing: %s\n",
                qPrintable(filename),
                qPrintable(file.errorString()));
        return false;
    }

    file.write(data->readAll());
    file.close();
*/
    return true;
}

void DownloadManager::sslErrors(const QList<QSslError> &sslErrors)
{
#ifndef QT_NO_SSL
    foreach (const QSslError &error, sslErrors)
        fprintf(stderr, "SSL error: %s\n", qPrintable(error.errorString()));
#else
    Q_UNUSED(sslErrors);
#endif
}

void DownloadManager::downloadFinished(QNetworkReply *reply)
{
    QString url = reply->url().toString();
    if (reply->error())
    {
        qDebug() << "Download failed: " << url;
    }
    else
    {
        if (m_pendingImages.contains(url))
        {
            ImageItem item = m_pendingImages[url];
            QByteArray pic = reply->readAll();
            QImage img = QImage::fromData(pic);
            item.setImage(img);
            m_pendingImages.remove(url);
            emit imageDownloaded(item);
        }
        else
        {
            for (int i = 0; i < m_currentDownloads.size(); i++)
            {
                if (m_currentDownloads[i]->url().toString() == reply->url().toString())
                {
                    QByteArray data = reply->readAll();
                    m_currentDownloads.removeAt(i);
                    emit jsonDownloaded(QString(data));
                    break;
                }
            }
        }
    }
    reply->deleteLater();
    if (m_currentDownloads.size() == 0)
        emit downloadsFinished();
}
