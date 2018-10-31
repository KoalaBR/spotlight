#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QPicture>
#ifdef REVERSE_IMAGE
    #include <QWebEngineView>
#endif
#include <QDesktopServices>

#include "dialogrename.h"
#include "ui_dialogrename.h"

DialogRename *dlgRename = nullptr;      ///< For Callback storeHTML()

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
    connect(ui->leTitle,    SIGNAL(textChanged(QString)), this, SLOT(slotTitleChanged(QString)));
    connect(ui->pbRename,   SIGNAL(clicked(bool)),        this, SLOT(slotRenameClicked()));
    connect(ui->tobSuggest, SIGNAL(clicked(bool)),        this, SLOT(slotReverseSearch()));
#ifdef REVERSE_IMAGE
    connect(&m_webView,     SIGNAL(loadFinished(bool)),   this, SLOT(slotReverseSearchFinished(bool)));
#endif
    connect(ui->tobShowUrl, SIGNAL(clicked(bool)),        this, SLOT(slotShowUrl()));
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

void DialogRename::slotReverseSearch(void)
{
#ifdef REVERSE_IMAGE
    ui->stwReverseSearch->setCurrentIndex(1);
    QString url   = m_reverseSearch.prepareImageSearch(m_item, m_baseDir);
    qDebug().noquote() << "Reverse url:" << url;
    if (url.isEmpty())
        return;
    m_webView.load(QUrl(url));
#endif
}

void storeHtml(QString html)
{
//    qDebug().noquote() << html;
    dlgRename->updateResult(html);
}

void DialogRename::slotReverseSearchFinished(bool ok)
{
#ifdef REVERSE_IMAGE
    if (ok)
    {
        dlgRename = this;
        ui->stwReverseSearch->setCurrentIndex(2);
        m_webView.page()->toHtml(&storeHtml);
//        m_reverseSearch.getResult("");

    }
    else ui->stwReverseSearch->setCurrentIndex(0);
#else
    Q_UNUSED(ok);
#endif
}

void DialogRename::slotShowUrl()
{
#ifdef REVERSE_IMAGE
    QDesktopServices::openUrl(m_webView.url());
#endif
}

void DialogRename::updateResult(QString html)
{
    QStringList result;
    // Suggestion start with  <a class="_gUb" href="/search...">suggestion</a>
    int pos = html.indexOf("<a class=\"_gUb\" href=");
    if (pos >= 0)
    {
        QString suggestion = html.mid(pos);
        pos = suggestion.indexOf(">");
        if (pos >= 0)
            suggestion = suggestion.mid(pos+1);
        pos = suggestion.indexOf("</a");
        if (pos >= 0)
            suggestion = suggestion.left(pos);
        result << suggestion;
    }
    // Other results <h3 class="r"><a href="http...." ...>suggestion</a>
    pos = html.indexOf("<h3 class=\"r\"><a href=\"http");
    if (pos >= 0)
    {
        do
        {
            QString suggestion = html.mid(pos+15);
            html = suggestion;
            pos = suggestion.indexOf(">");
            if (pos >= 0)
                suggestion = suggestion.mid(pos+1);
            pos = suggestion.indexOf("</");
            if (pos >= 0)
                suggestion = suggestion.left(pos);
            result << suggestion;
            pos = html.indexOf("<h3 class=\"r\"><a href=\"http");
        }
        while (pos >= 0);
    }
    ui->cmbSuggestions->clear();
    ui->cmbSuggestions->addItems(result);
}
