#ifndef CARTDIALOG_H
#define CARTDIALOG_H

#include <QDialog>
#include <QtSql>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlRelationalTableModel>
#include <QSqlRelationalDelegate>

namespace Ui {
class CartDialog;
}

class CartDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CartDialog(QWidget *parent = nullptr);
    ~CartDialog();
    void setDataBase(QSqlDatabase &base);
    void setCart_id(int cart);

public slots:
    void onItemAddedToCart(const QVariant& data);
    void updateCart();

private:
    Ui::CartDialog *ui;
    QSqlDatabase db;
    QSqlRelationalTableModel *model;
    int cart_id;
};

#endif // CARTDIALOG_H
