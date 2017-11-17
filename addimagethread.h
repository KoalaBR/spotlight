#ifndef ADDIMAGETHREAD_H
#define ADDIMAGETHREAD_H

#include <QObject>
#include <QThread>
#include <QTableWidget>

#include "database.h"
#include "imageitem.h"


enum class Command {CMD_INIT, CMD_IDLE, CMD_ADDIMAGE, CMD_CLEAR, CMD_SHUTDOWN};

class AddImageThread : public QThread
{
    Q_OBJECT
public:
    AddImageThread(Database *db, QTableWidget *over);

    void    doInit(Filter f);
    void    doClear(void);
    void    doAddImage(ImageItem item, bool newPic = false);
    void    doShutdown(void);

    ImageItem   getItem(int index);

protected:
    void    run() override;
signals:
    void        signalAddImage(QTableWidgetItem*, int, int, int);
private:
    void        initOverview(Filter f);
    void        addImage(ImageItem item, bool newPic = false);
    void        clearBackground(void);
    QTableWidgetItem    *createTableItem(ImageItem item);

    Database        *m_db;
    QTableWidget    *m_overview;
    QList<ImageItem> m_images;
    int              m_row, m_col, m_height;
    Command          m_currCMD;
    Filter           m_currFilter;
    ImageItem        m_currImage;
    bool             m_currNewPic;
};



#endif // ADDIMAGETHREAD_H
