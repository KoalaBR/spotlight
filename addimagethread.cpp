#include "addimagethread.h"

#include <QDebug>

AddImageThread::AddImageThread(Database *db, QTableWidget *over) : QThread(nullptr)
{
    m_db = db;
    m_id = 0;
    m_overview = over;
    setCurrentCmd(ThreadCommand::CMD_IDLE);
    m_showFlat = true;
}

void AddImageThread::doInit(Filter f)
{
    m_currFilter = f;
    int cols     = 4;
    m_overview->setColumnCount(cols);
    setCurrentCmd(ThreadCommand::CMD_INIT);
}

void AddImageThread::doShowTag(int id)
{
    m_id = id;
    setCurrentCmd(ThreadCommand::CMD_INIT);
}

void AddImageThread::doAddNextImage(void)
{
    if (m_nextImage < m_images.size())
    {
        setCurrentCmd(ThreadCommand::CMD_NEXT_IMAGE);

    }
    else
    {
        emit signalAllImagesAdded();
    }

}

void AddImageThread::doClear(void)
{
    setCurrentCmd(ThreadCommand::CMD_CLEAR);

}

void AddImageThread::doShutdown(void)
{
    setCurrentCmd(ThreadCommand::CMD_SHUTDOWN);

}

void AddImageThread::doShowAsFlat(bool flat)
{
    m_showFlat = flat;
}

void AddImageThread::doAddImage(ImageItem item, bool newPic)
{
    m_currImage  = item;
    m_currNewPic = newPic;
    m_nextImage++;
    setCurrentCmd(ThreadCommand::CMD_ADDIMAGE);
}

void AddImageThread::initOverview(Filter f)
{
    m_col = 0;
    m_row = 0;
    m_height = 0;
    m_images.clear();
    // Flat display (just as a grid of images)?
    if (m_showFlat)
        m_images = m_db->getImages(f);    // ok, get list of images currently stored in the db
    else
    if (m_id > 0)
        m_images = m_db->getImagesByTag(f, m_id);
    // Quit if we don't have images for flat mode
    if ((m_images.size() == 0) && m_showFlat)
        return;
    if (m_showFlat)
        initOverviewFlat();
    else showLevel(m_id);
}

void AddImageThread::initOverviewFlat(void)
{
    m_nextImage = 1;
    if (m_images.size() > 0)
    {
        ImageItem item = m_images[0];
        addImage(item);
    }
}

void AddImageThread::setCurrentCmd(ThreadCommand cmd)
{
    m_currCMD = cmd;
}

ThreadCommand AddImageThread::getCurrentCmd()
{
    ThreadCommand cmd = m_currCMD;
    return cmd;
}

/**
 * @brief AddImageThread::showLevel
 * show either the folders according to the existing tags (level = 0)
 * or show the images plus an entry for "back to top level" (level = 1)
 * @param images
 * @param level  if 0 show tags as folders otherwise show images from list plus "up"
 */
void AddImageThread::showLevel(int level)
{
    if (level == 0)
    {
        QList<Tag> tags = m_db->getTags();

        for (int i = 0; i < tags.size(); i++)
        {
            ImageItem item;
            item.setId(tags[i].id);
            item.setTitle(tags[i].tag);
            if (tags[i].id != 2)
                addImage(item, false, DisplayCommand::DIS_TAG);
            else addImage(item, false, DisplayCommand::DIS_DEL);
            if (getCurrentCmd() == ThreadCommand::CMD_SHUTDOWN)
                break;
        }
        emit signalAllImagesAdded();
    }
    else
    {
        ImageItem item;
        item.setTitle(tr("Zurück"));
        addImage(item, false, DisplayCommand::DIS_UP);
        initOverviewFlat();
    }
}

void AddImageThread::addImage(ImageItem item, bool newPic, DisplayCommand cmd)
{
    QTableWidgetItem *pic = createTableItem(item, cmd);
    if (newPic)
    {
        m_images.append(item);
        pic->setData(C_AIT_Index, m_images.size()-1);
    }
    else pic->setData(C_AIT_Index, m_nextImage);
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
            if (item != nullptr)
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
    setCurrentCmd(ThreadCommand::CMD_INIT);
}

void AddImageThread::run()
{
    forever
    {
        ThreadCommand cmd = getCurrentCmd();
        switch(cmd)
        {
            case ThreadCommand::CMD_IDLE:
                 this->usleep(10);
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
            case ThreadCommand::CMD_NEXT_IMAGE:
                 if (m_nextImage < m_images.size())
                 {
                     addImage(m_images[m_nextImage], false);
                     m_nextImage++;
                 }
                 m_currCMD = ThreadCommand::CMD_IDLE;
                 break;
        }
    }
}

QTableWidgetItem *AddImageThread::createTableItem(ImageItem item, const DisplayCommand cmd)
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
    else
    if (cmd == DisplayCommand::DIS_DEL)
    {
        QImage img(":/icons/folderbin.png");
        pic->setData(Qt::UserRole +2, static_cast<int>(DisplayCommand::DIS_TAG));
        pic->setData(Qt::DecorationRole, img.scaled(64,64));
        pic->setData(Qt::DisplayRole, item.title());
        pic->setData(Qt::UserRole +3, item.id());
        pic->setData(Qt::TextAlignmentRole, Qt::AlignBottom);
    }
    return pic;
}
