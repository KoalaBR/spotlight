#ifndef ADDIMAGETHREAD_H
#define ADDIMAGETHREAD_H

#include <QObject>
#include <QThread>
#include <QTableWidget>

#include "database.h"
#include "imageitem.h"

class AddImageThread : public QThread
{
public:
    AddImageThread(Database *db, QTableWidget *over);

    void        initOverview(Filter f);
    void        addImage(ImageItem item, bool newPic = false);
    void        clearBackground(void);
    ImageItem   getItem(int index);
signals:
    void    signalAddImage(ImageItem item);
private:
    QTableWidgetItem    *createTableItem(ImageItem item);

    Database        *m_db;
    QTableWidget    *m_overview;
    QList<ImageItem> m_images;
    int              m_row, m_col, m_height;
};

#endif // ADDIMAGETHREAD_H
