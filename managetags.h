#ifndef MANAGETAGS_H
#define MANAGETAGS_H

#include "database.h"

#include <QDialog>

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

private:
    Ui::ManageTags *ui;
    Database       *m_db;
    QList<Tag>      m_tags;
};

#endif // MANAGETAGS_H
