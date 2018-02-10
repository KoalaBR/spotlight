#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H
#include <QObject>
#include <QNetworkReply>

#include "imageitem.h"

#define MINOPENSSLVERSIONNUMBER 0x100010af // == OpenSSL Version 1.0.1j

class DownloadManager: public QObject
{
    Q_OBJECT

public:
    DownloadManager();
    void    downloadImage(const ImageItem item);
    void    downloadJSON(const QUrl &url);

public slots:
    void    downloadFinished(QNetworkReply *reply);
    void    sslErrors(const QList<QSslError> &errors);
signals:
    void    imageDownloaded(ImageItem item);
	void	SSL_not_supported(void);
    void    jsonDownloaded(QString content);
    void    downloadsFinished(void);
private:
	bool	checkSSLSupport(void);				///< Emits SSL_not_supported() if SSL can't be used

	QNetworkAccessManager       m_manager;
	QList<QNetworkReply *>      m_currentDownloads;
	QMap<QString, ImageItem>    m_pendingImages;
	bool						m_checked;		///< Check for SSL support only once (if m_checked == false
};
#endif // FILEDOWNLOADER_H
