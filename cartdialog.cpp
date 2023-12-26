#include "cartdialog.h"
#include "ui_cartdialog.h"

CartDialog::CartDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CartDialog)
{
    ui->setupUi(this);

    model = new QSqlRelationalTableModel (this);
    query = new QSqlQuery(db);
    updateCart();
    ui->tableView->setModel(model);
    ui->tableView->verticalHeader()->hide();
    model->setHeaderData(0, Qt::Horizontal, tr("покупатель"));
    model->setHeaderData(1, Qt::Horizontal, tr("товар"));
    model->setHeaderData(2, Qt::Horizontal, tr("кол-во"));
    model->setEditStrategy(QSqlRelationalTableModel::OnManualSubmit);
    ui->tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
}

CartDialog::~CartDialog()
{
    delete ui;
}

void CartDialog::setDataBase(QSqlDatabase &base){
    db = base;
}

void CartDialog::setCart_id(int cart)
{
    cart_id = cart;
}

void CartDialog::onItemAddedToCart(const QVariant &data)
{
    query->prepare("INSERT INTO cart_product (cart_id, product_id) VALUES (:cart_id, :product_id)");
    query->bindValue(":cart_id", cart_id);
    query->bindValue(":product_id", data.toInt());
    if(!query->exec()){
        qDebug() << query->lastError();
    }
}

void CartDialog::updateCart()
{
    model->setQuery("SELECT customer.name AS customer, product.name AS product, COUNT(cart_product.product_id) AS count "
                    "FROM cart_product LEFT JOIN cart ON cart_id = cart.id "
                    "LEFT JOIN customer ON customer_id = customer.id "
                    "LEFT JOIN product ON product_id = product.id "
                    "WHERE cart_product.cart_id=" + QString::number(cart_id) + " "
                    "GROUP BY cart_product.cart_id, cart_product.product_id", db);
}

void CartDialog::on_tableView_clicked(const QModelIndex &index)
{
    int row = index.row();
    curr_row = row;
    int column = 1;
    QVariant data = index.sibling(row, column).data();
    QString product_name = data.toString();
    query->exec("SELECT * FROM product WHERE name='" + product_name + "';");
    if (query->next()){
        product_id = query->record().value(0).toInt();
    }
}


void CartDialog::on_deleteButton_clicked()
{
    query->prepare("DELETE FROM cart_product "
                   "WHERE product_id = :product_id "
                   "AND ROWID IN "
                   "(SELECT ROWID FROM cart_product WHERE product_id = :product_id LIMIT 1);");
    query->bindValue(":product_id", product_id);
    if(!query->exec()){
        qDebug() << query->lastError();
    }
    updateCart();
}


void CartDialog::on_buyButton_clicked()
{
    query->prepare("DELETE FROM cart_product "
                   "WHERE product_id = :product_id "
                   "AND ROWID IN "
                   "(SELECT ROWID FROM cart_product WHERE product_id = :product_id LIMIT 1);");
    query->bindValue(":product_id", product_id);
    if(!query->exec()){
        qDebug() << query->lastError();
    }

    query->prepare("INSERT INTO ordered_product (cart_id, product_id) VALUES (:cart_id, :product_id);");
    query->bindValue(":cart_id", cart_id);
    query->bindValue(":product_id", product_id);
    if(!query->exec()){
        qDebug() << query->lastError();
    }
    updateCart();
}

