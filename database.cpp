#include "database.h"
#include <QVariant>
#include <QCryptographicHash>
#include <QBuffer>
#include <QDebug>
#include <QSqlError>

Database::Database()
{

}

Database::~Database()
{
    m_db.close();
}

bool Database::openDatabase(void)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("spotlight.db");
    if (!m_db.open())
    {
        qDebug() << "Fehler 1" << m_db.lastError().text();
        return false;
    }
    else
    {
        // Need to create table
        QString sql = "CREATE TABLE IF NOT EXISTS picdata (id INTEGER primary key autoincrement, "
                "url varchar(200), md5 varchar(250), deleted integer, desc varchar(250), "
                "source integer, thumb blob, title varchar(200), days integer, portrait integer)";
        QSqlQuery query(m_db);
        if (query.exec(sql))
            return true;
        else
        {
            qDebug() << "Fehler 2 "<<m_db.lastError().text();
            return false;
        }

    }
}

/**
 * @brief Database::canDownloadImage
 * can this item be downloaded?
 * @param item
 * @return
 */
bool Database::canDownloadImage(ImageItem item)
{
    if (item.source() == Source::SRC_BING)
    {
        // Must rely on title or days here, as the url is
        // always an offset in days from the current day
        return true;
    }
    else
    {
        QSqlQuery query(m_db);
        QString sql = "Select deleted from picdata where source=%1 and url=%2";
        sql = sql.arg(static_cast<int>(item.source())).arg(item.url());
        if (!query.exec(sql))
            return true;
        else
        {
            // If we have a result, we either don't want it or
            // already have it downloaded (as it is stored in the db).
            // So no download necessary
            if (!query.next())
                return true;
            else return false;
        }
    }
}

void Database::addImage(ImageItem &item)
{
    QSqlQuery query(m_db);
    QImage img = item.image();
    if (item.isPortrait())
    {
        img = img.scaledToHeight(150);
    }
    else
    {
        img = img.scaledToWidth(150);
    }
    item.setImage(img);
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    img.save(&buffer, "JPG");
    QString md5 = QString(QCryptographicHash::hash(ba, QCryptographicHash::Md5).toHex());
    QString sql = "insert into picdata(url, md5, deleted, source, title, days, desc, thumb, portrait) ";
    sql += "values(:1, :2, 0, :3, :4, 0, :5, :imagedata, :6)";
    query.prepare(sql);
    query.bindValue(":1", item.url());
    query.bindValue(":2", md5);
    query.bindValue(":3", static_cast<int>(item.source()));
    query.bindValue(":4", item.title());
    query.bindValue(":5", item.description());
    query.bindValue(":imagedata", ba);
    if (item.isPortrait())
        query.bindValue(":6", 1);
    else query.bindValue(":6", 0);
    query.exec();
}

QList<ImageItem> Database::getImages(Filter f)
{
    QSqlQuery query(m_db);
    QString sql = "select url, md5, deleted, source, title, days, thumb, desc, portrait from picdata ";
    if (f == Filter::FI_DELETED_ONLY)
        sql += "where deleted=1";
    if (f == Filter::FI_IMAGES_ONLY)
        sql += "where deleted=0";
    QList<ImageItem> list;
    if (query.exec(sql))
    {
        while (query.next())
        {
             QByteArray byteArray = query.value(6).toByteArray();
             QImage  img = QImage::fromData(byteArray);
             QString url = query.value(0).toString();
             QString md5 = query.value(1).toString();
             int     del = query.value(2).toInt();
             Source src  = static_cast<Source>(query.value(3).toInt());
             QString title = query.value(4).toString();
             QString desc  = query.value(7).toString();
             bool   portrait = false;
             if (query.value(8).toInt() != 0)
                 portrait = true;
             ImageItem item(title, url, desc, portrait, src);
             item.setImage(img);
             item.setDeleted(del);
             list.append(item);
        }
    }
    return list;
}

void Database::deleteImage(ImageItem item)
{
    QSqlQuery query(m_db);
    QString sql = "Update picdata set deleted=1 where ";
    if (item.source() == Source::SRC_BING)
    {
        // Can't use the url, need to rely on days
    }
    else
    {
        sql += "url=:1 and title=:2 and source=:3";
        query.prepare(sql);
        query.bindValue(":1", item.url());
        query.bindValue(":2", item.title());
        query.bindValue(":3", static_cast<int>(item.source()));
    }
    query.exec();
}
