#include "reverseimagesearch.h"

#include <QHttpMultiPart>
#include <QNetworkReply>
#include <QFile>
#include <QDebug>

#ifdef Q_OS_LINUX
    #include <curl/curl.h>
    #include <curl/easy.h>
#endif

ReverseImageSearch::ReverseImageSearch(QObject *parent) : QObject(parent)
{
    curl_global_init(CURL_GLOBAL_ALL);
    m_chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */
    m_chunk.size = 0;    /* no data at this point */
}

ReverseImageSearch::~ReverseImageSearch()
{

}


 /*
  curl -vv -s -F "image_url=" -F "image_content=" -F "filename="
      -F "h1=en"  -F "bih=179" -F "biw=1600" -F "encoded_image=@/home/prog/Bilder/download/spotlight/landscape/landscape_CrystalRiver_Colorado.jpg" https://www.google.co.in/searchbyimage/upload
  */

#ifdef Q_OS_LINUX

QString G_response;

static size_t writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    G_response += (char*) contents;
    return G_response.length();
}

QString ReverseImageSearch::prepareImageSearch(ImageItem item, QString baseDir)
{
    m_curl = curl_easy_init();
    curl_mime *form = NULL;
    curl_mimepart *field = NULL;
    struct curl_slist *headerlist = NULL;
    static const char buf[] = "Expect:";
    CURLcode res;
    if (m_curl)
    {
        G_response = "";
        /* Create the form */
        form = curl_mime_init(m_curl);
        // Set the user agent
        curl_easy_setopt(m_curl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64)");
        // Content-Disposition: form-data; name="image_url"
        field = curl_mime_addpart(form);
        curl_mime_name(field, "image_url");
        // Content-Disposition: form-data; name="image_content"
        field = curl_mime_addpart(form);
        curl_mime_name(field, "image_content");
        // Content-Disposition: form-data; name="filename"
        field = curl_mime_addpart(form);
        curl_mime_name(field, "filename");
        // Content-Disposition: form-data; name="h1" =en
        field = curl_mime_addpart(form);
        curl_mime_name(field, "h1");
        curl_mime_data(field, "en", CURL_ZERO_TERMINATED);
        // Content-Disposition: form-data; name="bih" =179
        field = curl_mime_addpart(form);
        curl_mime_name(field, "bih");
        curl_mime_data(field, "179", CURL_ZERO_TERMINATED);
        // Content-Disposition: form-data; name="biw"
        field = curl_mime_addpart(form);
        curl_mime_name(field, "biw");
        curl_mime_data(field, "1600", CURL_ZERO_TERMINATED);

        /* Content-Disposition: form-data; name="encoded_image"; filename="image.jpg"*/
        field = curl_mime_addpart(form);
        curl_mime_name(field, "encoded_image");
        QString filename = baseDir + item.filename();
//        curl_mime_filedata(field, filename.toStdString().c_str());
        curl_mime_filedata(field, "/tmp/image.jpg");


        /* initialize custom header list */
        headerlist = curl_slist_append(headerlist, buf);
        /* what URL that receives this POST */
        curl_easy_setopt(m_curl, CURLOPT_URL, C_RIS_URL);
        curl_easy_setopt(m_curl, CURLOPT_MIMEPOST, form);

        // Setup handler for response data
        curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &G_response);
        curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, writeMemoryCallback);
        /* Perform the request, res will get the return code */
        res = curl_easy_perform(m_curl);
        /* Check for errors */
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        /* always cleanup */
        curl_easy_cleanup(m_curl);

        /* then cleanup the form */
        curl_mime_free(form);
        /* free slist */
        curl_slist_free_all(headerlist);
        return getRedirectUrl(G_response);
    }
    return "";
}


#else

/**
 * This would be the Qt approach. However the request always adds some
 * headers to the request which makes Google issue a redirect to the
 * front page. I haven't found a way to remove those additional headers yet.
 *
 * So libcurl to the rescue
 * @brief ReverseImageSearch::prepareImageSearch
 * @param item
 */
void ReverseImageSearch::prepareImageSearch(const ImageItem item)
{
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart formPart;
    formPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"image_url\""));
    multiPart->append(formPart);
    formPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"image_content\""));
    multiPart->append(formPart);
    formPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"filename\""));
    multiPart->append(formPart);
    formPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"h1\""));
    formPart.setBody("en");
    multiPart->append(formPart);
    formPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"bih\""));
    formPart.setBody("179");
    multiPart->append(formPart);
    formPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"biw\""));
    formPart.setBody("1600");
    multiPart->append(formPart);
    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"encoded_image\"; filename=\"image.jpg\""));
    QFile *file = new QFile("/tmp/image.jpg");
    file->open(QIODevice::ReadOnly);
    imagePart.setBodyDevice(file);
    file->setParent(multiPart);
    multiPart->append(imagePart);

//    QUrl url("http://httpbin.org/anything");
//    QUrl url("http://localhost");
    QUrl url(C_RIS_URL);
    QNetworkRequest request(url);
    request.setRawHeader("MIME-Version", QByteArray());
    request.setRawHeader("Accept-Encoding", QByteArray());

    QNetworkReply *reply = m_manager.post(request, multiPart);
    qDebug() << request.rawHeaderList();
    connect(&m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotRequestFinished(QNetworkReply*)));
    multiPart->setParent(reply);
    qDebug() << multiPart;
}
#endif

void ReverseImageSearch::getResult(QString url)
{
    url = "http://www.google.co.in/search?tbs=sbi:AMhZZiuYgFkATS2GTMuT_1lTz-FEk8VRYvArlxavXJiHfH-kgoPHYsvkVMT0am1B5WYg1qZrEBvasLDn5PMAMnQqBqXLlP9EpzjcVlOLfbm2Nwy5sxEs0-K9C6r2zGLAzRfAfYozLgaUto68MnW3ZU703ToV5h2yPD3fcgvAKQq6R9bE4EBU4npAdybXhXlYdAtHDfhN8v0I5ULnhX2lJtKI-8xsqkBeUN5E9DbuzFmVChmurRDHCaye6qxRLVxqp-eVHuoOCUrF_14nRDsAcOk_1oKkH3cUcDdkYoxALEcvpaY-5JM58k3CM_1iMxJhYsiYWsw6r173olSappvbCZQP0UDzSEc7_1C_1dVw&amp;h1=en&amp;bih=179&amp;biw=1600";
    QNetworkRequest request(url);
    request.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8");

    reply = m_manager.get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(slotFinishedGet()));
}


void ReverseImageSearch::slotRequestFinished(QNetworkReply *reply)
{
    qDebug() << "slotfinished()";
    if (isRedirected(reply))
    {
        // We got a 302, so let's get the final url and post it again
        qDebug() << "Got 302 " << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        QString html = reply->readAll();
        html = getRedirectUrl(html);
        QUrl url(html);
        QNetworkRequest request(url);
//        m_manager.get(request);
    }
    else
    {
        qDebug() << "Reply" << reply->url().toString();
        qDebug().nospace().noquote() << reply->readAll();
    }

//    qDebug() << "Answer" << reply->readAll();
    delete reply;
}

void ReverseImageSearch::slotFinishedGet()
{
    qDebug() << "Reply" << reply->request().url().toString();
    qDebug().nospace().noquote() << reply->readAll();
}

bool ReverseImageSearch::isRedirected(QNetworkReply *reply)
{
    QVariant var = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (var.isNull() || (!var.isValid()))
        return false;
    if (var.toInt() == 302)
        return true;
    else return false;
}

QString ReverseImageSearch::getRedirectUrl(QString html)
{
    int pos = html.toLower().indexOf("HREF=\"http", 0, Qt::CaseInsensitive);
    if (pos < 0)
        return "";
    html = html.mid(pos+5);
    pos = html.indexOf("\">");
    if (pos < 0)
        return "";
    html = html.left(pos);
    return html;
}
