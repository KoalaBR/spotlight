#ifndef REVERSEIMAGESEARCH_H
#define REVERSEIMAGESEARCH_H

#include "imageitem.h"

#include <QObject>
#include <QNetworkAccessManager>

#include <curl/curl.h>


#define C_RIS_URL "http://www.google.co.in/searchbyimage/upload"   ///< URL for image upload

/**
 * @brief The ReverseImageSearch class
 *
 * Reverse image search is a service offered by Google. You upload an image to the
 * server (\ref C_RIS_SearchURL) and it returns a token which you use to query google
 * again in order to get the search result.
 */

typedef struct
{
    void    *memory;
    size_t   size;
} T_RIS_Memory;

class ReverseImageSearch : public QObject
{
    Q_OBJECT
public:
    explicit ReverseImageSearch(QObject *parent = nullptr);
    ~ReverseImageSearch();

    QString prepareImageSearch(ImageItem item, QString baseDir);  ///< Create image upload
    void getResult(QString url);

public slots:
    void slotRequestFinished(QNetworkReply *reply);
    void slotFinishedGet(void);
private:
    bool    isRedirected(QNetworkReply *reply);     ///< check whether we got a 302 moved answer
    QString getRedirectUrl(QString html);           ///< get the redirection url from the response
    QString copyImageToTemp(ImageItem item, QString baseDir);

    CURL                    *m_curl;
    QNetworkAccessManager    m_manager;
    T_RIS_Memory             m_chunk;
    QNetworkReply *reply;
 };
extern QString     G_response;

#endif // REVERSEIMAGESEARCH_H
