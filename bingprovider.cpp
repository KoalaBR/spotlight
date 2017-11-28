#include "bingprovider.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <QDebug>

BingProvider::BingProvider(QPlainTextEdit *edit) : AbstractProvider (edit)
{
    m_nextRequest = true;
}

QString BingProvider::createFirstRequest()
{
    printLine("Searching on bing");
    QString url = "http://www.bing.com/HPImageArchive.aspx?format=js&idx=0&n=8&mkt=de-DE";
    m_nextRequest = false;
    return url;
}

bool BingProvider::canCreateNextRequest()
{
    return m_nextRequest;
}

QList<ImageItem> BingProvider::getItemList(QByteArray data)
{
    QList<ImageItem> list;
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject  obj  = doc.object();
    QJsonArray images = obj["images"].toArray();
    for (int i = 0; i < images.size(); i++)
    {
        QJsonObject jsItem = images[i].toObject();
        QString url = jsItem["url"].toString();
        QString title = jsItem["copyright"].toString();
        int pos = title.lastIndexOf(" (");
        title = title.left(pos);
        ImageItem item(title, "http://www.bing.com" + url, "", false, Source::SRC_BING);
        pos = url.lastIndexOf('.');
        if (pos >= 0)
        {
            url = url.left(pos+1);
            pos = url.lastIndexOf("_");
            QString format = url.mid(pos);
            QStringList fl = format.split("x");
            int width = 0;
            int height = 0;
            if (fl.size() == 2)
            {
                width  = fl[0].toInt();
                height = fl[1].toInt();
            }
            item.setHeight(height);
            item.setWidth(width);
            item.setPortrait((height > width));
            list.append(item);
        }
    }
    return list;
}
