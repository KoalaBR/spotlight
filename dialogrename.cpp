#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QPicture>

#include "dialogrename.h"
#include "ui_dialogrename.h"

DialogRename::DialogRename(const ImageItem item, Database *db, QString baseDir, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogRename)
{
    m_db   = db;
    m_item = item;
    m_baseDir = baseDir;
    ui->setupUi(this);
    ui->leTitle->setText(item.title());
    ui->lblImage->setPixmap(QPixmap::fromImage(m_item.image()));
    connect(ui->leTitle,  SIGNAL(textChanged(QString)), this, SLOT(slotTitleChanged(QString)));
    connect(ui->pbRename, SIGNAL(clicked(bool)),        this, SLOT(slotRenameClicked()));
    connect(ui->tobSuggest, SIGNAL(clicked(bool)),      this, SLOT(slotReverseSearch()));
}

DialogRename::~DialogRename()
{
    delete ui;
}

QString DialogRename::getNewTitle(void)
{
    return ui->leTitle->text();
}

bool DialogRename::isTitleUsed(QString title)
{
    ImageItem newItem = m_item;
    newItem.setTitle(title);
    QString filename = m_baseDir + newItem.filename();
    QFile test(filename);
    return test.exists();
}

void DialogRename::slotTitleChanged(QString title)
{
    QPalette *palette = new QPalette();
    if (isTitleUsed(title.trimmed()))
    {
        palette->setColor(QPalette::Text,Qt::red);
        ui->pbRename->setEnabled(false);
    }
    else
    {
        palette->setColor(QPalette::Text, Qt::black);
        ui->pbRename->setEnabled(true);
    }
    ui->leTitle->setPalette(*palette);
}

void DialogRename::slotRenameClicked(void)
{
    QString title = ui->leTitle->text().trimmed();
    if (title.length() == 0)
        return;
    ImageItem newItem = m_item;
    newItem.setTitle(title);
    QString newFilename = m_baseDir + newItem.filename();
    QString oldFilename = m_baseDir + m_item.filename();
    QFile file(oldFilename);
    if (file.rename(newFilename))
    {
        if (m_db->setNewTitle(m_item, title))
        {
            accept();
        }
        else
        {
            // Undo rename, because we can't update the database
            QFile fileUndo(newFilename);
            if (!fileUndo.rename(oldFilename))
            {
                QMessageBox::critical(this, tr("Rename failed (Undo)"),
                                      tr("Database update failed but couldn't rename file back!?") +"\n" + fileUndo.errorString(),
                                      QMessageBox::Ok);
            }
        }
    }
    else
    {
        // Rename failed, just alert the user
        QMessageBox::critical(this, tr("Rename failed"),
                              tr("Rename failed. System says:") +"\n" + file.errorString(), QMessageBox::Ok);
    }
}

void DialogRename::slotReverseSearch()
{
    QString url   = m_reverseSearch.prepareImageSearch(m_item, m_baseDir);
    qDebug().noquote() << "Reverse url:" << url;
//    if (url.isEmpty())
//        return;
    m_reverseSearch.getResult("url");
//    qDebug() << "guess" <<guess;
}
