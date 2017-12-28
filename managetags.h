#ifndef MANAGETAGS_H
#define MANAGETAGS_H

#include "database.h"

#include <QDialog>
#include <QTableWidgetItem>

namespace Ui {
class ManageTags;
}

class ManageTags : public QDialog
{
    Q_OBJECT

public:
    explicit ManageTags(Database *db, QWidget *parent = 0);
    ~ManageTags();

public slots:
    void    slotItemChanged(QTableWidgetItem *current, QTableWidgetItem *);
    void    slotAddTag(void);
    void    slotDelTag(void);
protected:
    bool    eventFilter(QObject *obj, QEvent *event);
private:
    void    setupTags(void);            ///< fills the manage tags dialog with tag data
    void    updateTag(void);

    Ui::ManageTags *ui;
    Database       *m_db;
    QList<Tag>      m_tags;
};

#endif // MANAGETAGS_H
