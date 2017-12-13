#ifndef ADDIMAGETHREAD_H
#define ADDIMAGETHREAD_H

#include <QObject>
#include <QThread>
#include <QTableWidget>

#include "database.h"
#include "imageitem.h"


#define C_AIT_Index Qt::UserRole +1     ///< The index of the image in m_images
#define C_AIT_Type  Qt::UserRole +2

enum class ThreadCommand  {CMD_INIT, CMD_IDLE, CMD_ADDIMAGE, CMD_CLEAR, CMD_SHUTDOWN};  ///< what does the thread do next?

enum class DisplayCommand {DIS_TAG = 0,     ///< this one is a dir
                           DIS_UP  = 1,     ///< we are in a dir, get to top level
                           DIS_PIC = 2      ///< this is a picture
                          };

class AddImageThread : public QThread
{
    Q_OBJECT
public:
    AddImageThread(Database *db, QTableWidget *over);

    void    doAddImage(ImageItem item, bool newPic = false);
    void    doClear(void);
    void    doInit(Filter f);
    void    doShowTag(int id);
    void    doShowTopLevel(void);
    void    doShutdown(void);
    void    doShowAsFlat(bool flat);          ///< if true, just show the pics as they come in, otherwise show with tags as directory
    ImageItem   getItem(int index);
protected:
    void    run() override;
signals:
    void        signalAddImage(QTableWidgetItem*, int, int, int);
private:
    void        initOverview(Filter f);
    void        addImage(ImageItem item, bool newPic = false, DisplayCommand cmd=DisplayCommand::DIS_PIC);
    void        clearBackground(void);
    void        initOverviewFlat(QList<ImageItem> images);
    void        showLevel(QList<ImageItem> images, int level);
    QTableWidgetItem    *createTableItem(ImageItem item, DisplayCommand cmd = DisplayCommand::DIS_PIC);

    Database        *m_db;
    QTableWidget    *m_overview;
    QList<ImageItem> m_images;
    int              m_row, m_col, m_height;
    ThreadCommand    m_currCMD;
    Filter           m_currFilter;
    ImageItem        m_currImage;
    bool             m_currNewPic;

    bool             m_showFlat;            ///< do we sort our images by tags into folders (false) or do we just display them (true)?
    int              m_id;                  ///< Used only if m_showFlat = true: if 0 we show the tags else images for this tag id plus "back"
};



#endif // ADDIMAGETHREAD_H
