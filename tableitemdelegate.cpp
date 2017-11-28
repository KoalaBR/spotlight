#include "tableitemdelegate.h"
#include "addimagethread.h"

#include <QDebug>
#include <QPainter>
#include <QImage>

TableItemDelegate::TableItemDelegate(QObject* parent) : QStyledItemDelegate(parent)
{

}

TableItemDelegate::~TableItemDelegate()
{

}

void TableItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    DisplayCommand cmd = static_cast<DisplayCommand>(index.data(Qt::UserRole +2).toInt());
    if (cmd == DisplayCommand::DIS_PIC)
        QStyledItemDelegate::paint(painter, option, index);
    else
    {
        QString text = index.data(Qt::DisplayRole).toString();
        QRect   rect = option.rect;          // This is the max. space we have
        int  fheight = painter->fontMetrics().height()+4;
        QImage   img = index.data(Qt::DecorationRole).value<QImage>();
        int    xoffs = (rect.width() - img.width()) >> 1;
        int    yoffs = (rect.height() - fheight - img.height()) >> 1;
        int toffs = (rect.width() - painter->fontMetrics().width(text)) >> 1;
        painter->drawImage(rect.left() + xoffs, rect.top() +yoffs, img);
        painter->drawText(rect.left() + toffs, rect.bottom() -6, text);
    }

}
