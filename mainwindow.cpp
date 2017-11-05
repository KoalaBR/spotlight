#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QMap>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imageitem.h"

QMap<QString, ImageItem>    imageMap;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createCacheDirs();
    connect(ui->pbSearch,   SIGNAL(clicked()),                  this, SLOT(clickedSearch()));
    connect(&m_downloader,  SIGNAL(jsonDownloaded(QString)),    this, SLOT(slotDownloadComplete(QString)));
    connect(&m_downloader,  SIGNAL(imageDownloaded(ImageItem)), this, SLOT(slotImageDownloadComplete(ImageItem)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clickedSearch()
{
    // Start url erstellen
    QUrl url = QUrl(createFirstRequest());
    ui->teShowActions->clear();
    printLine("Lade Startbild\n");
    m_downloader.downloadJSON(url);
}

/**
 * @brief MainWindow::slotDownloadComplete
 * Will be called once a none image file has been downloaded (probably JSON or
 * HTML)
 * @param content
 */
void MainWindow::slotDownloadComplete(QString content)
{
    printLine("Daten geladen - Ermittle URLs");
    QList<ImageItem> itemList = getItemList(content.toUtf8());
    for (int i = 0; i < itemList.size(); i++)
    {
        int orientation = ui->cmbOrientation->currentIndex();
        ImageItem item = itemList[i];
        if (item.isPortrait() && (orientation != 1))    // 0 = Portrait, 2 = both
        {
            // download data
            m_downloader.downloadImage(item);
        }
        else
        if ((!item.isPortrait()) && (orientation != 0)) // 1 = landscape, 2 = both
        {
            // download
            m_downloader.downloadImage(item);
        }
    }
}

void MainWindow::printLine(QString line)
{
    ui->teShowActions->moveCursor(QTextCursor::Start);
    ui->teShowActions->insertPlainText(line+"\r");
}

QList<ImageItem> MainWindow::getItemList(QByteArray data)
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


QList<ImageItem> MainWindow::decodeJsonList(QString value)
{
    QJsonParseError jsErr;
    QJsonDocument doc(QJsonDocument::fromJson(value.toUtf8(), &jsErr));
    QJsonObject  item = doc.object();
    QList<ImageItem> result;
    QJsonObject   jsImage= item.value("ad").toObject();
    result.append(getImageItem(jsImage));
    return result;
}

QList<ImageItem> MainWindow::getImageItem(QJsonObject image)
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

QString MainWindow::createFirstRequest(void)
{
    // lo = 86774
    QString url = "https://arc.msn.com/v3/Delivery/Cache?pid=209567&fmt=json&";
    url += "rafb=0&ua=WindowsShellClient%2F0&disphorzres=2560&dispvertres=1440";
    url += "&lo=80217&pl=de-DE&lc=de-DE&ctry=de&time=";
    QDateTime time = QDateTime::currentDateTime();
   // url += time.toString("yyyyMMdd")+"T" + time.toString("hhmmss") + "Z";
    // url += "2016"+time.toString("MMdd")+"T" + time.toString("hhmmss") + "Z";
    url += "20161205T022103Z";
    return url;
}

void MainWindow::createCacheDirs(void)
{
    QString path = QString("download") + QDir::separator() + "spotlight";
    QDir md = QDir();
    md.mkpath(path);
    path = QString("download") + QDir::separator() + "bing";
    md.mkpath(path);

}

int count = 0;  /// Nur zum Test!!!!!!!!

void MainWindow::slotImageDownloadComplete(ImageItem item)
{
    qDebug() << "Download Ende: "<< item.url();
    QString filename = QString("download");
    if (item.source() == Source::SRC_SPOTLIGHT)
        filename += QDir::separator() + QString("spotlight");
    else filename += QDir::separator() + QString("bing");

//    filename += QDir::separator() + item.filename();
    filename = QString("C:\\tmp\\test%1.jpg").arg(count++);
    qDebug() << filename;
    item.image().save(filename);
}
