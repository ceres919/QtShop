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

private slots:
    void on_tableView_clicked(const QModelIndex &index);
    void on_deleteButton_clicked();

    void on_buyButton_clicked();

private:
    Ui::CartDialog *ui;
    QSqlDatabase db;
    QSqlQuery *query;
    QSqlRelationalTableModel *model;
    int cart_id;
    int product_id;
    int curr_row;
};

#endif // CARTDIALOG_H
