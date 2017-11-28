#ifndef IMAGEITEM_H
#define IMAGEITEM_H
#include <QString>
#include <QImage>

enum class Source {SRC_BING, SRC_CHROMECAST, SRC_SPOTLIGHT};
enum class Format {FOR_PORTRAIT, FOR_LANDSCAPE, FOR_ANY};

class ImageItem
{
public:
    ImageItem(QString title, QString url, QString description, bool portrait, Source src=Source::SRC_SPOTLIGHT);
    ImageItem() { m_url = ""; m_deleted = false;}

    QString title() const;
    QString url() const;
    QString description() const;
    bool    isPortrait() const;
    bool    isDeleted(void);
    void    setSource(Source src);
    void    setImage(QImage img);
    void    setDeleted(int del);
    void    setPortrait(bool port);
    void    setId(const int id);
    void    setTitle(const QString title);

    int     id(void) const;
    int     width(void) const;
    int     height(void) const;
    void    setWidth(int width);
    void    setHeight(int height);
    Source  source(void);
    QString filename(void);
    QImage  image(void);

private:
    QString     m_title;
    QString     m_url;
    QString     m_description;
    int         m_width;
    int         m_height;
    int         m_deleted;
    bool        m_portrait;
    QImage      m_img;
    Source      m_src;
    int         m_id;
};

#endif // IMAGEITEM_H
