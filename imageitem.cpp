#include "imageitem.h"
#include <QDebug>
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
    QString filename = "portrait_";
    if (!isPortrait())
        filename = "landscape_";
    if (m_title != "")
    {
        QString name = m_title;
        name = name.replace(',','_');
        name = name.replace(" ", "");
        filename += name + ".jpg";
    }
    else
    {
        QString dummy = QString(QCryptographicHash::hash(m_url.toUtf8(),QCryptographicHash::Md5).toHex());
        dummy = dummy.left(20);
        filename += dummy + ".jpg";
    }
    filename = filename.normalized(QString::NormalizationForm_KD);
    filename = filename.remove(QRegExp("[^a-zA-Z0-9._\\s]"));
    return filename;
}

QImage ImageItem::image()
{
    return m_img;
}
