#ifndef DIALOGRENAME_H
#define DIALOGRENAME_H

#include <QDialog>

namespace Ui {
class DialogRename;
}

class DialogRename : public QDialog
{
    Q_OBJECT

public:
    explicit DialogRename(QWidget *parent = 0);
    ~DialogRename();

private:
    Ui::DialogRename *ui;
};

#endif // DIALOGRENAME_H
