#ifndef DIALOGRENAME_H
#define DIALOGRENAME_H

#include "imageitem.h"
#include "database.h"
#include "reverseimagesearch.h"

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
public slots:
    void        slotTitleChanged(QString title);                    ///< Called once the text in the edit field changed
    void        slotRenameClicked(void);                            ///< User requested rename
    void        slotReverseSearch(void);                            ///< Start reverse image search (button clicked)
private:
    bool        isTitleUsed(QString title);   ///< is there a filename collision with the requested title?

    Ui::DialogRename *ui;
    ImageItem       m_item;
    Database       *m_db;
    QString         m_baseDir;
    ReverseImageSearch  m_reverseSearch;
};

#endif // DIALOGRENAME_H
