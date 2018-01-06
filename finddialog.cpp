#include <QTableWidget>
#include <QString>

#include "finddialog.h"
#include "ui_finddialog.h"

FindDialog::FindDialog(Database *db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindDialog)
{
    ui->setupUi(this);
    ui->tbwOverview->setColumnWidth(0,180);
    m_db = db;
    connect(ui->leSearch, SIGNAL(textChanged(QString)), this, SLOT(slotSearch(QString)));
    connect(ui->tbClear,  SIGNAL(clicked()),            this, SLOT(slotClear()));
    m_list = m_db->getImages(Filter::FI_ALL);
}

FindDialog::~FindDialog()
{
    delete ui;
}

void FindDialog::on_pushButton_clicked(void)
{
    accept();
}

void FindDialog::slotClear()
{
    ui->leSearch->clear();
    ui->leSearch->setFocus();
}

void FindDialog::slotSearch(QString text)
{
    if (text.length() < 3)
        return;
    text = text.toLower();
    ui->tbwOverview->clearContents();
    int count = 0;
    for (int i = 0; i < m_list.size(); i++)
    {
        ImageItem img  = m_list[i];
        QString  title = img.title().toLower();
        QString  desc  = img.description();

        if ((title.indexOf(text) < 0) &&
            (desc.indexOf(text) < 9))
            continue;
        ui->tbwOverview->setRowCount(count+1);
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setData(Qt::ToolTipRole, title);
        item->setData(Qt::DecorationRole, img.image());
        ui->tbwOverview->setItem(count, 0, item);
        QString tags = getTagList(img);
        item = new QTableWidgetItem();
        item->setText(tags);
        item->setToolTip(tags);
        ui->tbwOverview->setItem(count, 1, item);
        item = new QTableWidgetItem();
        item->setText(img.title());
        ui->tbwOverview->setItem(count, 2, item);
        ui->tbwOverview->setRowHeight(count, img.image().height() + 6);
        count++;
    }
}

QString FindDialog::getTagList(const ImageItem item)
{
    QString result = "";
    QList<Tag> list = m_db->getTagsForImage(item);
    for (int i = 0; i < list.size(); i++)
    {
        result += list[i].tag;
        if (i < list.size() -1)
            result += ", ";
    }
    return result;
}
