#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>
#include <QTableWidgetItem>
#include "database.h"

namespace Ui {
class FindDialog;
}

class FindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindDialog(Database *db,QWidget *parent = nullptr);
    ~FindDialog();

signals:
    void signalShowImage(ImageItem img);

private slots:
    void slotShowImage(QTableWidgetItem *item);
    void on_pushButton_clicked();
    void slotClear();
    void slotSearch(QString text);

private:
    QString     getTagList(const ImageItem item);

    Ui::FindDialog  *ui;
    Database        *m_db;
    QList<ImageItem> m_list;
};

#endif // FINDDIALOG_H
