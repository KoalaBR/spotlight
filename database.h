#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include "imageitem.h"

typedef struct {
    QString     tag;
    int         id;
} Tag;

enum class Filter {FI_ALL, FI_DELETED_ONLY, FI_IMAGES_ONLY };

class Database
{
public:
    Database();
    ~Database();

    QList<ImageItem>    getImages(const Filter f);
    void                addImage(ImageItem &item);
    void                deleteImage(ImageItem item);
    bool                canDownloadImage(ImageItem item);
    bool                openDatabase(void);
    ImageItem           getRandomImage(void);
    QList<Tag>          getTags(void);
    bool                isTagUsed(const int tagid, const int imgid);        ///< Returns true if the given tag is used for the image

private:

    QSqlDatabase    m_db;
};

#endif // DATABASE_H
