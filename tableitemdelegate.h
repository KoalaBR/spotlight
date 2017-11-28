#ifndef TABLEITEMDELEGATE_H
#define TABLEITEMDELEGATE_H

#include <QStyledItemDelegate>

class TableItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    TableItemDelegate(QObject* parent = 0);
   ~TableItemDelegate();

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
//    QSize sizeHint(const QStyleOptionViewItem &option,
//                   const QModelIndex &index) const override;
};

#endif // TABLEITEMDELEGATE_H
