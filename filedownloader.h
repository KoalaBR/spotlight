#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H
#include <QObject>
#include <QNetworkReply>

#include "imageitem.h"

class DownloadManager: public QObject
{
    Q_OBJECT
    QNetworkAccessManager       m_manager;
    QList<QNetworkReply *>      m_currentDownloads;
    QMap<QString, ImageItem>    m_pendingImages;

public:
    DownloadManager();
    void    downloadImage(const ImageItem item);
    void    downloadJSON(const QUrl &url);

    QString saveFileName(const QUrl &url);

public slots:
    void    downloadFinished(QNetworkReply *reply);
    void    sslErrors(const QList<QSslError> &errors);
signals:
    void    imageDownloaded(ImageItem item);
    void    jsonDownloaded(QString content);
    void    downloadsFinished(void);
};
#endif // FILEDOWNLOADER_H
