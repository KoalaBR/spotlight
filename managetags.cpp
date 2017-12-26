#include "managetags.h"
#include "ui_managetags.h"

ManageTags::ManageTags(Database *db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ManageTags)
{
    m_db = db;
    ui->setupUi(this);
    connect(ui->pbClose, SIGNAL(clicked(bool)), this, SLOT(close()));
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

ManageTags::~ManageTags()
{
    delete ui;
}
