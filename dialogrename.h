#ifndef DIALOGRENAME_H
#define DIALOGRENAME_H

#include "imageitem.h"
#include "database.h"
#ifdef REVERSE_IMAGE
    #include "reverseimagesearch.h"
    #include <QWebEngineView>
#endif

#include <QDialog>

namespace Ui {
class DialogRename;
}

class DialogRename : public QDialog
{
    Q_OBJECT

public:
    explicit DialogRename(const ImageItem item, Database *db,
                          QString baseDir, QWidget *parent = 0);
    ~DialogRename();
    QString         getNewTitle(void);
    void            updateResult(QString html);
public slots:
    void        slotTitleChanged(QString title);                    ///< Called once the text in the edit field changed
    void        slotRenameClicked(void);                            ///< User requested rename
    void        slotReverseSearch(void);                            ///< Start reverse image search (button clicked)
    void        slotReverseSearchFinished(bool ok);                 ///< Reverse image finished (succesful true / false)
    void        slotShowUrl(void);
private:
    bool        isTitleUsed(QString title);   ///< is there a filename collision with the requested title?

    Ui::DialogRename *ui;
    ImageItem       m_item;
    Database       *m_db;
    QString         m_baseDir;
#ifdef REVERSE_IMAGE
    ReverseImageSearch  m_reverseSearch;
    QWebEngineView  m_webView;
#endif
};

#endif // DIALOGRENAME_H
