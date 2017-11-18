#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include "imageitem.h"

enum class Filter {FI_ALL, FI_DELETED_ONLY, FI_IMAGES_ONLY };

class Database
{
public:
    Database();
    ~Database();

    QList<ImageItem>    getImages(Filter f);
    void                addImage(ImageItem &item);
    void                deleteImage(ImageItem item);
    bool                canDownloadImage(ImageItem item);
    bool                openDatabase(void);
    ImageItem           getRandomImage();
private:

    QSqlDatabase    m_db;
};

#endif // DATABASE_H
