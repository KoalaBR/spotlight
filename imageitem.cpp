#include "imageitem.h"
#include <QDebug>
#include <QDir>
#include <QCryptographicHash>

ImageItem::ImageItem(QString title,          QString url,
                     QString description="", bool    portrait=false, Source src)
{
    m_title = title;
    m_url   = url;
    m_src   = src;
    m_description = description;
    m_portrait    = portrait;
    m_deleted     = 0;
    m_id          = 0;
}

QString ImageItem::title() const
{
    return m_title;
}

QString ImageItem::url() const
{
    return m_url;
}

QString ImageItem::description() const
{
    return m_description;
}

bool ImageItem::isPortrait() const
{
    return m_portrait;
}

void ImageItem::setSource(Source src)
{
    m_src = src;
}

void ImageItem::setImage(QImage img)
{
    m_img = img;
}

bool ImageItem::isDeleted()
{
    if (m_deleted == 0)
        return false;
    else return true;
}

void ImageItem::setDeleted(int del)
{
    m_deleted = del;
}

void ImageItem::setPortrait(bool port)
{
    m_portrait = port;
}

void ImageItem::setId(const int id)
{
    m_id = id;
}

int ImageItem::id() const
{
    return m_id;
}

int ImageItem::width(void) const
{
    return m_width;
}

void ImageItem::setWidth(int width)
{
    m_width = width;
}

int ImageItem::height() const
{
    return m_height;
}

void ImageItem::setHeight(int height)
{
    m_height = height;
}

Source ImageItem::source(void)
{
    return m_src;
}

QString ImageItem::filename(void)
{
    QString srcDirs[] = { "bing", "chromecast", "spotlight"};
    int index = static_cast<int>(m_src);
    QString path = "download";
    path += QDir::separator() + srcDirs[index] + QDir::separator();
    if (!isPortrait())
    {
        path += QString("landscape") + QDir::separator();
        path += "landscape_";
    }
    else
    {
        path += QString("portrait") + QDir::separator();
        path += "portrait_";
    }
    QString fname = "";
    if (m_title != "")
    {
        QString name = m_title;
        name  = name.replace(',','_');
        name  = name.replace(" ", "");
        fname = name + ".jpg";
        fname = fname.normalized(QString::NormalizationForm_KD);
        fname = fname.remove(QRegExp("[^a-zA-Z0-9._\\s]"));
    }
    else
    {
        fname = QString(QCryptographicHash::hash(m_url.toUtf8(),QCryptographicHash::Md5).toHex());
        fname = fname.left(20);
        fname+= fname + ".jpg";
    }
    path = path + fname;
    return path;
}

QImage ImageItem::image()
{
    return m_img;
}
