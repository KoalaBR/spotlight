#include "addimagethread.h"

#include <QDebug>

AddImageThread::AddImageThread(Database *db, QTableWidget *over) : QThread(NULL)
{
    m_db = db;
    m_id = 0;
    m_overview = over;
    m_currCMD  = ThreadCommand::CMD_IDLE;
    m_showFlat = true;
}

void AddImageThread::doInit(Filter f)
{
    m_currFilter = f;
    int cols     = 4;
    m_overview->setColumnCount(cols);
    m_currCMD    = ThreadCommand::CMD_INIT;
}

void AddImageThread::doShowTag(int id)
{
    m_id = id;
    m_currCMD = ThreadCommand::CMD_INIT;
}

void AddImageThread::doClear(void)
{
    m_currCMD = ThreadCommand::CMD_CLEAR;
}

void AddImageThread::doShutdown(void)
{
    m_currCMD = ThreadCommand::CMD_SHUTDOWN;
}

void AddImageThread::doShowAsFlat(bool flat)
{
    m_showFlat = flat;
}

void AddImageThread::doAddImage(ImageItem item, bool newPic)
{
    m_currImage  = item;
    m_currNewPic = newPic;
    m_currCMD    = ThreadCommand::CMD_ADDIMAGE;
}

void AddImageThread::initOverview(Filter f)
{
    m_col = 0;
    m_row = 0;
    m_height = 0;
    m_images.clear();
    QList<ImageItem> images;
    if (m_showFlat)
        images = m_db->getImages(f);
    else
    if (m_id > 0)
        images = m_db->getImagesByTag(f, m_id);
    if ((images.size() == 0) && m_showFlat)
        return;
    if (m_showFlat)
        initOverviewFlat(images);
    else showLevel(images, m_id);
}

void AddImageThread::initOverviewFlat(QList<ImageItem> images)
{
    for (int i = 0; i < images.size(); i++)
    {
        ImageItem item = images[i];
        addImage(item);
        if (m_currCMD == ThreadCommand::CMD_SHUTDOWN)
            break;
    }

}

/**
 * @brief AddImageThread::showLevel
 * show either the folders according to the existing tags (level = 0)
 * or show the images plus an entry for "back to top level" (level = 1)
 * @param images
 * @param level  if 0 show tags as folders otherwise show images from list plus "up"
 */
void AddImageThread::showLevel(QList<ImageItem> images, int id)
{
    if (id == 0)
    {
        QList<Tag> tags = m_db->getTags();
        for (int i = 0; i < tags.size(); i++)
        {
            ImageItem item;
            item.setId(tags[i].id);
            item.setTitle(tags[i].tag);
            addImage(item, false, DisplayCommand::DIS_TAG);
            if (m_currCMD == ThreadCommand::CMD_SHUTDOWN)
                break;
        }
    }
    else
    {
        ImageItem item;
        item.setTitle(tr("Zurück"));
        addImage(item, false, DisplayCommand::DIS_UP);
        initOverviewFlat(images);
    }
}

void AddImageThread::addImage(ImageItem item, bool newPic, DisplayCommand cmd)
{
    m_images.append(item);
    QTableWidgetItem *pic = createTableItem(item, cmd);
    pic->setData(C_AIT_Index, m_images.size()-1);
    if (newPic)
        pic->setBackgroundColor(Qt::green);
    if (item.isDeleted())
        pic->setBackgroundColor(Qt::red);
    if (cmd == DisplayCommand::DIS_PIC)
    {
        if (item.image().height() > m_height)
            m_height = item.image().height();
    }
    else if (90 > m_height)
        m_height = 90;
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

void AddImageThread::doShowTopLevel(void)
{
    m_id = 0;
    m_currCMD = ThreadCommand::CMD_INIT;
}

void AddImageThread::run()
{
    forever
    {
        switch(m_currCMD)
        {
            case ThreadCommand::CMD_IDLE:
                 this->msleep(250);
                 break;
            case ThreadCommand::CMD_CLEAR:
                 this->clearBackground();
                 m_currCMD = ThreadCommand::CMD_IDLE;
                 break;
            case ThreadCommand::CMD_SHUTDOWN:
                 return;
            case ThreadCommand::CMD_INIT:
                 initOverview(m_currFilter);
                 m_currCMD = ThreadCommand::CMD_IDLE;
                 break;
            case ThreadCommand::CMD_ADDIMAGE:
                 addImage(m_currImage, m_currNewPic);
                 m_currCMD = ThreadCommand::CMD_IDLE;
                 break;
        }
    }
}

QTableWidgetItem *AddImageThread::createTableItem(ImageItem item, DisplayCommand cmd)
{
    QTableWidgetItem *pic = new QTableWidgetItem();
    pic->setData(Qt::ToolTipRole, item.title());
    pic->setData(Qt::UserRole +2, static_cast<int>(cmd));
    if (cmd == DisplayCommand::DIS_PIC)
    {
        pic->setData(Qt::DecorationRole, item.image());
        pic->setText(item.title());
    }
    else
    if (cmd == DisplayCommand::DIS_TAG)
    {
        QImage img(":/icons/folder.png");
        pic->setData(Qt::DecorationRole, img.scaled(64,64));
        pic->setData(Qt::DisplayRole, item.title());
        pic->setData(Qt::UserRole +3, item.id());
        pic->setData(Qt::TextAlignmentRole, Qt::AlignBottom);
    }
    else
    if (cmd == DisplayCommand::DIS_UP)
    {
        QImage img(":/icons/folderup.png");
        pic->setData(Qt::DecorationRole, img.scaled(64,64));
        pic->setData(Qt::DisplayRole, item.title());
        pic->setData(Qt::TextAlignmentRole, Qt::AlignBottom);
    }
    return pic;
}
