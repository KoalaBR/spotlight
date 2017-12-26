#include "database.h"
#include <QVariant>
#include <QCryptographicHash>
#include <QBuffer>
#include <QDebug>
#include <QSqlError>

Database::Database()
{
    m_filter = Filter::FI_ALL;
}

Database::~Database()
{
    m_db.close();
}

void Database::setFilter(Filter fi)
{
    m_filter = fi;
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
        if (!query.exec(sql))
        {
            qDebug() << "Fehler 2 "<<m_db.lastError().text();
            return false;
        }
//        sql = "Create table if not exists tagimg(tagid INTEGER, imageid INTEGER)";
        sql = "create table if not exists tagimg(tagid integer not null, picid integer not null, primary key(tagid, picid))";
        if (!query.exec(sql))
        {
            qDebug() << "Failed to create tagimg";
        }
        sql = "CREATE TABLE IF NOT EXISTS tags(id INTEGER primary key autoincrement, "
                "name varchar(50))";
        if (!query.exec(sql))
        {
            qDebug() << "Error: creating tag table failed " << m_db.lastError().text();
            return false;
        }
        QStringList  tags;
        sql = "Select count(*) as anz from tags";
        query.exec(sql);
        if (query.next())
        {
            if (query.value(0).toInt() > 0)
                return true;
        }
        tags << "Neu"      << "Gelöscht" << "Landschaften"
             << "Tiere"    << "Bauwerke" << "Himmel" ;
        for (int i = 0; i < tags.length(); i++)
        {
            sql = "insert into tags(name) values('"+ tags[i]+"')";
            query.exec(sql);
        }

        return true;
    }
}

ImageItem Database::getRandomImage(void)
{
    QList<ImageItem> list = getImages(m_filter);
    if (list.size() == 0)
        return ImageItem();
    int index = rand() % list.size();
    return list[index];
}

/**
 * @brief Database::canDownloadImage
 * can this item be downloaded?
 * @param item
 * @return
 */
bool Database::canDownloadImage(ImageItem item)
{
    QSqlQuery query(m_db);
    QString sql = "Select count(deleted) from picdata where source=:1 and url=:2";
    query.prepare(sql);
    query.bindValue(":1", static_cast<int>(item.source()));
    query.bindValue(":2", item.url());
    if (!query.exec())
        return true;
    else
    {
        bool    ok;
        // If we have a result, we either don't want it or
        // already have it downloaded (as it is stored in the db).
        // So no download necessary

        if (!query.next())
            return true;
        int count = query.value(0).toInt(&ok);
        if (!ok)
            return true;
        if (count == 0)
            return true;
        else return false;
    }
}

void Database::addImage(ImageItem &item)
{
    QSqlQuery query(m_db);
    QImage img = item.image();
    if (item.isPortrait())
    {
        img = img.scaledToHeight(150, Qt::SmoothTransformation);
    }
    else
    {
        img = img.scaledToWidth(150, Qt::SmoothTransformation);
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
    // Get the last inserted id for the table
    sql = "select seq from sqlite_sequence where name='picdata'";
    query.exec(sql);
    if (query.next())
        item.setId(query.value(1).toInt());
}

QList<ImageItem> Database::getImages(const Filter f)
{
    QSqlQuery query(m_db);
    QString sql = "select url, md5, deleted, source, title, days, thumb, desc, portrait,id from picdata ";
    if (f == Filter::FI_LANDSCAPE)
        sql += "where portrait=0";
    if (f == Filter::FI_PORTRAIT)
        sql += "where portrait=1";
    QList<ImageItem> list;
    if (query.exec(sql))
    {
        while (query.next())
        {
             QByteArray byteArray = query.value(6).toByteArray();
             QImage  img = QImage::fromData(byteArray);
             QString url = query.value(0).toString();
//             QString md5 = query.value(1).toString();
             int     del = query.value(2).toInt();
             Source src  = static_cast<Source>(query.value(3).toInt());
             QString title = query.value(4).toString();
             QString desc  = query.value(7).toString();
             bool   portrait = false;
             if (query.value(8).toInt() != 0)
                 portrait = true;
             int id = query.value(9).toInt();
             ImageItem item(title, url, desc, portrait, src);
             item.setImage(img);
             item.setId(id);
             item.setDeleted(del);
//             bool add = false;
//             if (form == ImageFormat::IF_ANY)
//                 add = true;
//             if ((form == ImageFormat::IF_LANDSCAPE) && (!portrait))
//                 add = true;
//             if ((form == ImageFormat::IF_PORTRAIT) && portrait)
//                 add = true;
//             if (add)
                list.append(item);
        }
    }
    return list;
}

/**
 * @brief Database::getImagesByTag
 * Gets the images by tagid.
 * @param f
 * @param tagid  1 = all items not tagged are "new", 2 = return all items which are deleted
 * @return
 */
QList<ImageItem> Database::getImagesByTag(const Filter f, int tagid)
{
    // select * from picdata where id in (select picid from tagimg where tagid=3)
    QSqlQuery query(m_db);
    QString sql = "select url, md5, deleted, source, title, days, thumb, desc, portrait,id from picdata ";
    if (tagid > 2)
    {
        sql += "where id in (select picid from tagimg where tagid=%1)";
        sql = sql.arg(tagid);
    }
    else
    if (tagid == 2)
        sql += "where deleted=1";
    else sql += "where id not in (select picid from tagimg) and deleted=0";
    if (f == Filter::FI_PORTRAIT)
        sql += " and portrait=1";
    if (f == Filter::FI_LANDSCAPE)
        sql += " and portrait=0";
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
             int id = query.value(9).toInt();
             ImageItem item(title, url, desc, portrait, src);
             item.setImage(img);
             item.setId(id);
             item.setDeleted(del);
             list.append(item);
        }
    }
    else qDebug() << query.lastError().text();
    return list;

}

void Database::deleteImage(ImageItem item)
{
    QSqlQuery query(m_db);
    QString sql = "Update picdata set deleted=1 where ";
    if (item.source() == Source::SRC_BING)
    {
        sql += QString("id = %1").arg(item.id());
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

void Database::tagImage(bool checked, int tagid, int imgid)
{
    QSqlQuery query(m_db);
    QString sql = "";
    if (checked)
        sql = "insert into tagimg(tagid, picid) values(:1, :2)";
    else sql = "delete from tagimg where tagid=:1 and picid=:2";
    query.prepare(sql);
    query.bindValue(":1", tagid);
    query.bindValue(":2", imgid);
    query.exec();
}

QList<Tag> Database::getTags(void)
{
    QList<Tag> list;
    QString sql = "select id, name from tags";
    QSqlQuery query(m_db);
    if (query.exec(sql))
    {
        while (query.next())
        {
            int id = query.value(0).toInt();
            QString name = query.value(1).toString();
            Tag tag;
            tag.id = id;
            tag.tag = name;
            list.append(tag);
        }
    }
    return list;
}

bool Database::isTagUsed(const int tagid, const int imgid)
{
    QString sql = "Select count(*) from tagimg where tagid=:1 and picid=:2";
    QSqlQuery query(m_db);
    query.prepare(sql);
    query.bindValue(":1", tagid);
    query.bindValue(":2", imgid);
    query.exec();
    if (query.next())
    {
        if (query.value(0).toInt() > 0)
            return true;
        else return false;
    }
    else qDebug() << query.lastError().text();
    return false;
}
