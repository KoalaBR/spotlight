#include "dialogrename.h"
#include "ui_dialogrename.h"

DialogRename::DialogRename(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogRename)
{
    ui->setupUi(this);
}

DialogRename::~DialogRename()
{
    delete ui;
}
