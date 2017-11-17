#include "addimagethread.h"

#include <QDebug>


AddImageThread::AddImageThread(Database *db, QTableWidget *over) : QThread(NULL)
{
    m_db = db;
    m_overview = over;
    m_currCMD = Command::CMD_IDLE;
}

void AddImageThread::doInit(Filter f)
{
    m_currFilter = f;
    m_overview->clear();
    int cols     = 4;
    m_overview->setColumnCount(cols);
    m_currCMD    = Command::CMD_INIT;
}

void AddImageThread::doClear(void)
{
    m_currCMD = Command::CMD_CLEAR;
}

void AddImageThread::doShutdown(void)
{
    m_currCMD = Command::CMD_SHUTDOWN;
}

void AddImageThread::doAddImage(ImageItem item, bool newPic)
{
    m_currImage  = item;
    m_currNewPic = newPic;
    m_currCMD    = Command::CMD_ADDIMAGE;
}

void AddImageThread::initOverview(Filter f)
{
    m_col = 0;
    m_row = 0;
    m_height = 0;
    QList<ImageItem> images = m_db->getImages(f);
    if (images.size() == 0)
        return;
    int rows = images.size() / 4;
    if (rows == 0)
    {
        if (images.size() > 0)
            rows = 1;
    }
    else
    if (images.size() % rows != 0)
        rows++;
    for (int i = 0; i < images.size(); i++)
    {
        ImageItem item = images[i];
        addImage(item);
        if (m_currCMD == Command::CMD_SHUTDOWN)
            break;
    }
}

void AddImageThread::addImage(ImageItem item, bool newPic)
{
    m_images.append(item);
    QTableWidgetItem *pic = createTableItem(item);
    pic->setData(Qt::UserRole +1, m_images.size()-1);
    if (newPic)
        pic->setBackgroundColor(Qt::green);
    if (item.isDeleted())
        pic->setBackgroundColor(Qt::red);
    if (item.image().height() > m_height)
        m_height = item.image().height();
    emit signalAddImage(pic, m_row, m_col, m_height);
    m_col++;
    if (4 == m_col)
    {
        m_row++;
        m_height = 0;
        m_col = 0;
    }
//    m_overview->setRowHeight(m_row, m_height+8);
}

void AddImageThread::clearBackground()
{
    for (int r = 0; r < m_overview->rowCount(); r++)
    {
        for (int c = 0; c < m_overview->columnCount(); c++)
        {
            QTableWidgetItem *item = m_overview->item(r, c);
            if (item != NULL)
                item->setBackgroundColor(Qt::white);
        }
    }
}

ImageItem AddImageThread::getItem(int index)
{
    if (index < 0)
        return ImageItem();
    if (index >= m_images.size())
        return ImageItem();
    return m_images[index];
}

void AddImageThread::run()
{
    forever
    {
        switch(m_currCMD)
        {
            case Command::CMD_IDLE:
                 this->usleep(1000);
                 break;
            case Command::CMD_CLEAR:
                 this->clearBackground();
                 m_currCMD = Command::CMD_IDLE;
                 break;
            case Command::CMD_SHUTDOWN:
                 return;
            case Command::CMD_INIT:
                 initOverview(m_currFilter);
                 m_currCMD = Command::CMD_IDLE;
                 break;
            case Command::CMD_ADDIMAGE:
                 addImage(m_currImage, m_currNewPic);
                 m_currCMD = Command::CMD_IDLE;
                 break;
        }
    }
}

QTableWidgetItem *AddImageThread::createTableItem(ImageItem item)
{
    QTableWidgetItem *pic = new QTableWidgetItem();
    pic->setData(Qt::DecorationRole, item.image());
    pic->setData(Qt::ToolTipRole, item.title());
    pic->setText(item.title());
    return pic;
}
