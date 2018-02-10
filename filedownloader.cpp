#include <QNetworkReply>
#include <QImage>
#include <QFileInfo>
#include <QList>
#include <QSslError>

#include "filedownloader.h"
#include <imageitem.h>


DownloadManager::DownloadManager()
{
    connect(&m_manager, SIGNAL(finished(QNetworkReply*)),
            SLOT(downloadFinished(QNetworkReply*)));
	m_checked = false;
}

void DownloadManager::downloadJSON(const QUrl &url)
{
	if (!checkSSLSupport())
		return;
	QNetworkRequest request(url);
    QNetworkReply *reply = m_manager.get(request);

#ifndef QT_NO_SSL
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), SLOT(sslErrors(QList<QSslError>)));
#endif
    m_currentDownloads.append(reply);
}

void DownloadManager::downloadImage(const ImageItem item)
{
	if (!checkSSLSupport())
		return;
    QNetworkRequest request(item.url());
    QNetworkReply *reply = m_manager.get(request);

#ifndef QT_NO_SSL
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), SLOT(sslErrors(QList<QSslError>)));
#endif
    m_pendingImages.insert(item.url(), item);
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

bool DownloadManager::checkSSLSupport(void)
{
	if (m_checked)
		return true;
	m_checked = true;
	if (QSslSocket::supportsSsl())
	{
		long version = QSslSocket::sslLibraryVersionNumber();
		qDebug() << QSslSocket::sslLibraryVersionString()
			<< QString::number(version, 16);
		if (MINOPENSSLVERSIONNUMBER > version)
		{
			emit SSL_not_supported();
			return false;
		}
		else return true;
	}
	emit SSL_not_supported();
	return false;
}