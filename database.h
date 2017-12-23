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
enum class ImageFormat {IF_ANY, IF_PORTRAIT, IF_LANDSCAPE };

class Database
{
public:
    Database();
    ~Database();

    QList<ImageItem>    getImages(const Filter f);
    QList<ImageItem>    getImagesByTag(const Filter f, int tagid);
    void                addImage(ImageItem &item);
    void                deleteImage(ImageItem item);
    void                tagImage(bool checked, int tagid, int imgid);
    bool                canDownloadImage(ImageItem item);
    bool                openDatabase(void);
    ImageItem           getRandomImage();
    QList<Tag>          getTags(void);
    bool                isTagUsed(const int tagid, const int imgid);        ///< Returns true if the given tag is used for the image

private:

    QSqlDatabase    m_db;
};

#endif // DATABASE_H
