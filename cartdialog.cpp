#include "cartdialog.h"
#include "ui_cartdialog.h"

CartDialog::CartDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CartDialog)
{
    ui->setupUi(this);
}

CartDialog::~CartDialog()
{
    delete ui;
}

void CartDialog::setDataBase(QSqlDatabase base){
    db = base;
}

void CartDialog::setCart_id(int cart)
{
    cart_id = cart;
}
