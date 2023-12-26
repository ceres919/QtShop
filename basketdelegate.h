#ifndef BASKETDELEGATE_H
#define BASKETDELEGATE_H

#include <QStyledItemDelegate>
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

#define BASKET_BUTTON_COLUMN_INDEX 4

class BasketButtonDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    BasketButtonDelegate(QObject *parent = 0) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        if (index.column() == BASKET_BUTTON_COLUMN_INDEX) {
            QRect rect = option.rect;
            painter->setBrush(QColor::fromRgb(161, 125, 246));
            painter->drawRoundedRect(rect.adjusted(0, 35, 0, -35), 3, 3);
            painter->setPen(Qt::white);
            painter->drawText(rect, Qt::AlignCenter, "в корзину");
        }
        else
        {
            QStyledItemDelegate::paint(painter, option, index);
        }
    }

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
    {
        if (event->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (index.column() == BASKET_BUTTON_COLUMN_INDEX && option.rect.contains(mouseEvent->pos())) {
                emit addToBasketClicked(index.row());
                return true;
            }
        }

        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }

signals:
    void addToBasketClicked(int itemId);
};

#endif // BASKETDELEGATE_H
