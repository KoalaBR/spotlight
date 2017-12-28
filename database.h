#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include "imageitem.h"

typedef struct {
    QString     tag;
    int         id;
} Tag;


enum class Filter {FI_ALL, FI_LANDSCAPE, FI_PORTRAIT};   ///< What picture format to display (all, only portrait or only landscape)
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
    ImageItem           getRandomImage(void);
    QList<Tag>          getTags(void);
    bool                isTagUsed(const int tagid, const int imgid);        ///< Returns true if the given tag is used for the image
    void                setFilter(Filter fi);
    void                addTag(int id, QString name);                       ///< Add a new tag (id = -1) or change existing one (id > 0)
    void                deleteTag(int id);                                  ///< remove tag with given id from database
private:

    QSqlDatabase    m_db;
    Filter          m_filter;           ///< Which type of images will be displayed?
};

#endif // DATABASE_H
