#include "chromecastprovider.h"
#include <QDebug>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


ChromecastProvider::ChromecastProvider(QPlainTextEdit *edit) : AbstractProvider (edit)
{
    m_format = Format::FOR_ANY;
}



QString ChromecastProvider::createFirstRequest()
{
    return "https://clients3.google.com/cast/chromecast/home";
}

QList<ImageItem> ChromecastProvider::getItemList(QByteArray data)
{
    QList<ImageItem> list;
    QString page = QString(data);
    int pos = page.indexOf("JSON.parse");
    if (pos == -1)
        return list;
    page = page.mid(pos+12);
    pos = page.indexOf(")).");
    page = page.left(pos-1);
    page = page.mid(1);
    page = page.replace("\\x5b", "[");
    page = page.replace("\\x22", "\"");
    page = page.replace("\\/", "/");
    page = page.replace("\\n", "");
    page = page.replace("\\\\u003d", "=");
    page = page.replace("\\x5d", "]");
    QStringList slist = page.split("[\"");
    if (slist.size() < 1)
        return list;
    slist.removeFirst();
    slist.removeLast();
    for (int i = 0; i < slist.size();i++)
    {
        QString url = slist[i];
        pos = url.indexOf("=s");
        url = url.left(pos);
        if (url.toLower().startsWith("http"))
        {
            if ((m_format == Format::FOR_PORTRAIT) ||
                (m_format == Format::FOR_ANY))
            {
                ImageItem image("", url+"=s1080-w1080-h1920-p-k-no-nd-mv", "", true, Source::SRC_CHROMECAST);
                list.append(image);
            }
            if ((m_format == Format::FOR_LANDSCAPE) ||
                (m_format == Format::FOR_ANY))
            {
                ImageItem image("", url+"=s1920-w1920-h1080-p-k-no-nd-mv", "", false, Source::SRC_CHROMECAST);
                list.append(image);
                qDebug() << url;
            }
        }
        // Just a few pics, not all at once
        if (list.size() > 10)
            break;
    }
    return list;
}

void ChromecastProvider::setFormat(Format f)
{
    m_format = f;
}
