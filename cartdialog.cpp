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
