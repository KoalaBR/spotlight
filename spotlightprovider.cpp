#include "spotlightprovider.h"
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QDebug>

SpotlightProvider::SpotlightProvider(QPlainTextEdit *edit) : AbstractProvider(edit)
{

}

QString SpotlightProvider::createFirstRequest(void)
{
    printLine("Spotlight: Frage nach Bilderliste...\n");
    QString url = "https://arc.msn.com/v3/Delivery/Cache?pid=209567&fmt=json&";
    url += "rafb=0&ua=WindowsShellClient%2F0&disphorzres=2560&dispvertres=1440";
    url += "&lo=80217&pl=de-DE&lc=de-DE&ctry=de&time=";
    QDateTime time = QDateTime::currentDateTime();
    url += time.toString("yyyyMMdd")+"T" + time.toString("hhmmss") + "Z";
    // url += "2016"+time.toString("MMdd")+"T" + time.toString("hhmmss") + "Z";
    url += "20161205T022103Z";
    qDebug() << url;
    return url;
}

QList<ImageItem> SpotlightProvider::getItemList(QByteArray data)
{
    QList<ImageItem> result;
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject  obj = doc.object();
    QJsonObject batch= obj["batchrsp"].toObject();
    QJsonArray items = batch["items"].toArray();
    if (items.size() == 0)
    {
        QJsonObject err = batch["errors"].toObject();
        printLine("Fehler:" + err["msg"].toString());
        return result;
    }
    for (int i = 0; i < items.size(); i++)
    {
        QJsonObject jsItem = items[i].toObject();
        QString value = jsItem["item"].toString();
        QList<ImageItem> subItemList = decodeJsonList(value);
        result.append(subItemList);
    }
    return result;

}

QList<ImageItem> SpotlightProvider::decodeJsonList(QString value)
{
    printLine("Durchsuche Bilderliste...\n");
    QJsonParseError jsErr;
    QJsonDocument doc(QJsonDocument::fromJson(value.toUtf8(), &jsErr));
    QJsonObject  item = doc.object();
    QList<ImageItem> result;
    QJsonObject   jsImage= item.value("ad").toObject();
    result.append(getImageItem(jsImage));
    return result;
}

QList<ImageItem> SpotlightProvider::getImageItem(QJsonObject image)
{
    QList<ImageItem> result;
    // title_text: Überschrift
    // hs2_title_text: Beschreibung Zeile 1
    // hs2_cta_text:   Beschreibung Zeile 2
    // image_fullscreen_001_portrait: Array mit Bildinformationen (Hochformat)
    // image_fullscreen_001_landscape: Array mit Bildinformationen (Querformat)
    //    in diesem Array:
    //    w: Breite, h: Höhe, u =  Url zum Download, sha256: Checksumme
    //    filesize: Dateigröße in Bytes
    QJsonObject obj = image["title_text"].toObject();
    QString title =  obj["tx"].toString();
    obj = image["hs2_title_text"].toObject();
    QString text  = obj["tx"].toString();
    obj = image["hs2_cta_text"].toObject();
    text += "\n" +  obj["tx"].toString();
    QJsonObject jsImageP = image["image_fullscreen_001_portrait"].toObject();
    QJsonObject jsImageL = image["image_fullscreen_001_landscape"].toObject();
    if (!jsImageL.isEmpty())
    {
        printLine("Querformat: "+title);
        QString val = jsImageL["w"].toString();
        int  width  = val.toInt();
        val = jsImageL["h"].toString();
        int  height = val.toInt();
        QString url = jsImageL["u"].toString();
        ImageItem itemL(title, url, text, false);
        itemL.setWidth(width);
        itemL.setHeight(height);
        result.append(itemL);
    }
    if (!jsImageP.isEmpty())
    {
        printLine("Hochformat: "+title);
        QString val = jsImageP["w"].toString();
        int  width  = val.toInt();
        val = jsImageP["h"].toString();
        int  height = val.toInt();
        QString url = jsImageP["u"].toString();
        ImageItem itemP(title, url, text, true);
        itemP.setWidth(width);
        itemP.setHeight(height);
        result.append(itemP);
    }
    return result;
}
