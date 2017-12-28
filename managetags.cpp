#include <QDebug>
#include <QTableWidgetItem>
#include <QKeyEvent>

#include "managetags.h"
#include "ui_managetags.h"

ManageTags::ManageTags(Database *db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ManageTags)
{
    m_db = db;
    ui->setupUi(this);
    connect(ui->pbClose, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(ui->tbwTags, SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)), this, SLOT(slotItemChanged(QTableWidgetItem*, QTableWidgetItem*)));
    connect(ui->tbAdd,   SIGNAL(clicked(bool)), this, SLOT(slotAddTag()));
    connect(ui->tbDel,   SIGNAL(clicked(bool)), this, SLOT(slotDelTag()));
    setupTags();
    qApp->installEventFilter(this);
    if (m_tags.size() > 0)
        ui->tbwTags->selectRow(0);
}

ManageTags::~ManageTags()
{
    delete ui;
}

void ManageTags::setupTags(void)
{
    m_tags = m_db->getTags();
    ui->tbwTags->clearContents();
    ui->tbwTags->setRowCount(m_tags.size());
    for (int i = 0; i < m_tags.size(); i++)
    {
        QTableWidgetItem *item = new QTableWidgetItem(m_tags[i].tag);
        item->setData(Qt::UserRole, m_tags[i].id);
        ui->tbwTags->setItem(i,0, item);
        if ((m_tags[i].id == 1) ||  // New images here
            (m_tags[i].id == 2))    // Deleted images
        {
            QIcon icon(":/icons/application-certificate.png");
            item = new QTableWidgetItem(icon, "");
            ui->tbwTags->setItem(i, m_tags[i].id, item);
        }
    }
}

void ManageTags::slotItemChanged(QTableWidgetItem *current, QTableWidgetItem *)
{
    if (current != NULL)
    {
        ui->leLabel->setText(current->text());
        ui->leLabel->setFocus();
    }
}

void ManageTags::slotAddTag(void)
{
    QTableWidgetItem *item = new QTableWidgetItem(tr("New Tag"));
    int row = ui->tbwTags->rowCount();
    item->setData(Qt::UserRole, -1);
    ui->tbwTags->setRowCount(row+1);
    ui->tbwTags->setItem(row, 0, item);
    m_db->addTag(-1, tr("New Tag"));
    setupTags();
}

void ManageTags::slotDelTag(void)
{
    QList<QTableWidgetItem*> list = ui->tbwTags->selectedItems();
    if (list.size() == 0)
        return;
    ui->leLabel->clear();
    QTableWidgetItem *item = list.at(0);
    if (item != NULL)
    {
        int id = item->data(Qt::UserRole).toInt();
        if ((id == 1) || (id == 2))
            return;
        int row = ui->tbwTags->currentRow();
        ui->tbwTags->removeRow(row);
        m_db->deleteTag(id);
    }
}

bool ManageTags::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->leLabel && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *key = static_cast<QKeyEvent *>(event);
        if ((key->key() == Qt::Key_Return) ||
            (key->key() == Qt::Key_Enter))
        {
            updateTag();
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}

void ManageTags::updateTag(void)
{
    QString text = ui->leLabel->text();
    QTableWidgetItem *item = ui->tbwTags->currentItem();
    if (item != NULL)
    {
        int id = item->data(Qt::UserRole).toInt();
        item->setText(text);
        ui->leLabel->setText("");
        ui->tbwTags->clearSelection();
        ui->tbwTags->setFocus();
        m_db->addTag(id, text);
        setupTags();
    }
}
