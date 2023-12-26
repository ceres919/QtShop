#include "cartdialog.h"
#include "ui_cartdialog.h"

CartDialog::CartDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CartDialog)
{
    ui->setupUi(this);
    result = 0;
    model = new QSqlRelationalTableModel (this);
    query = new QSqlQuery(db);
    updateCart();
    ui->tableView->setModel(model);
    ui->tableView->verticalHeader()->hide();
    model->setHeaderData(0, Qt::Horizontal, tr("покупатель"));
    model->setHeaderData(1, Qt::Horizontal, tr("товар"));
    model->setHeaderData(2, Qt::Horizontal, tr("кол-во"));
    model->setHeaderData(3, Qt::Horizontal, tr("цена"));
    model->setEditStrategy(QSqlRelationalTableModel::OnManualSubmit);
    ui->tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);

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

void CartDialog::setDeliveryAddress(QString address)
{
    deliveryAddress = address;
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
    model->setQuery("SELECT customer.name AS customer, product.name AS product, "
                    "COUNT(cart_product.product_id) AS count, SUM(product.price) AS price "
                    "FROM cart_product LEFT JOIN cart ON cart_id = cart.id "
                    "LEFT JOIN customer ON customer_id = customer.id "
                    "LEFT JOIN product ON product_id = product.id "
                    "WHERE cart_product.cart_id=" + QString::number(cart_id) + " "
                    "GROUP BY cart_product.cart_id, cart_product.product_id", db);


    int sum = 0;
    for (int row=0; row<model->rowCount(); row++) {
        QModelIndex index = model->index(row, 3, QModelIndex());
        sum += index.data().toInt();
    }
    result = sum;
    ui->resultPrice->setText(QString::number(sum) + " рублей");
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

    if(result > 0){
        QMessageBox msgBox;
        msgBox.setText("Подтверждаете оформление заказа?");
        msgBox.setInformativeText("Адрес доставки: " + deliveryAddress);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        switch (ret) {
        case QMessageBox::Yes: {
                QString checkList;
                QString ktokupil;
                query->exec("SELECT customer.name AS customer, product.name AS product, "
                            "COUNT(cart_product.product_id) AS count, SUM(product.price) AS price "
                            "FROM cart_product LEFT JOIN cart ON cart_id = cart.id "
                            "LEFT JOIN customer ON customer_id = customer.id "
                            "LEFT JOIN product ON product_id = product.id "
                            "WHERE cart_product.cart_id=" + QString::number(cart_id) + " "
                            "GROUP BY cart_product.cart_id, cart_product.product_id");
                while(query->next()) {
                    ktokupil.clear();
                    QString item = query->record().value(1).toString() + " | (×"
                               + query->record().value(2).toString() + ") | "
                               + query->record().value(3).toString() + " руб.\n";
                    checkList.append(item);
                    ktokupil.append(query->record().value(0).toString());
                }
                QMessageBox msgBox2;
                msgBox2.setText("Сохранить распечатанный чек?");
                msgBox2.setInformativeText(checkList);
                msgBox2.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
                msgBox2.setDefaultButton(QMessageBox::Save);
                int ret2 = msgBox2.exec();
                switch (ret2) {
                case QMessageBox::Save:{
                        QString str = QFileDialog::getSaveFileName();
                        if (str.isEmpty()) {
                            QMessageBox::warning(this, "Error", "Файл не открыт!");
                            return;
                        }
                        QString out = "КАССОВЫЙ ЧЕК\n";
                        out += "[";
                        out += QDateTime::currentDateTime().toString();
                        out += "] ";
                        out += ktokupil;
                        out += "\n\n";
                        out += checkList;
                        out += "\n\n";
                        out += "ИТОГО: ";
                        out += QString::number(result);
                        out += " рублей\n";

                        QFile file(str);
                        if (file.open(QIODevice::WriteOnly))
                        {
                            QTextStream stream(&file);
                            stream << out;
                            file.flush();
                            file.close();
                            if (stream.status() != QTextStream::Ok)
                            {
                                qDebug() << "Ошибка записи файла";
                            }
                            else{
                                QMessageBox::warning(this, "", "Чек успешно сохранен!");
                            }
                        }
                    }
                    break;
                default:
                    break;
                }
                query->prepare("DELETE FROM cart_product "
                               "WHERE cart_id = :cart_id;");
                query->bindValue(":cart_id", cart_id);
                if(!query->exec()){
                    qDebug() << query->lastError();
                }
            }
            break;
        case QMessageBox::No:
            // Don't Save was clicked
            break;
        default:
            // should never be reached
            break;
        }

        updateCart();
    }
    else{
        QMessageBox::warning(this, "Ваша корзина пуста", "Невозможно оформить заказ");
    }
}

