#include "accountdialog.h"
#include "ui_accountdialog.h"

AccountDialog::AccountDialog(QWidget *parent, QSqlDatabase *db, QSqlRecord *account) :
    QDialog(parent),
    db(*db),
    account(*account),
    ui(new Ui::AccountDialog)
{
    ui->setupUi(this);
    ui->nameLineEdit->setText(account->value(1).toString());
    ui->emailLineEdit->setText(account->value(2).toString());

    query = new QSqlQuery(*db);

    connect(ui->logOutPushButton, SIGNAL(clicked()), this, SLOT(logOutButton_clicked()));
    connect(ui->saveChangesPushButton, SIGNAL(clicked()), this, SLOT(saveButton_clicked()));

}

AccountDialog::~AccountDialog()
{
    delete ui;
}

void AccountDialog::logOutButton_clicked(){

    this->hide();
    emit logOutSignal();
}

void AccountDialog::saveButton_clicked(){
    QRegExp name("^[А-ЯЁ][а-яё]+(?:\\s+[А-ЯЁ][а-яё]+){0,2}(?:\\s+[А-ЯЁ]\\.)?(?:\\s+[А-ЯЁ]\\.)?$");
    QRegExp email("^((?!\\.)[\\w\\-_.]*[^.])(@\\w+)(\\.\\w+(\\.\\w+)?[^.\\W])$");
    QRegExp password("^[A-Za-z\\d]{8,}");
    if(!password.exactMatch(ui->newPasswordLineEdit->text()) || !name.exactMatch(ui->nameLineEdit->text())
        || !email.exactMatch(ui->emailLineEdit->text())){
        QMessageBox::critical(this, "Ошибка", "Проверьте правильность ввода имени, пароля или email."
                                          "Пароль должен состоять минимум из 8 символов.");
        return;
    }
    if(ui->oldPasswordLineEdit->text() != account.value(3).toString()){
        QMessageBox::critical(this, "Ошибка", "Старый пароль не совпадает");
        return;
    }

    int t =  QMessageBox::question(this,"Внимание!", "После изменения данных, Вам необходимо повторно авторизоваться в приложении."
                                        "Вы уверены, что хотите сохранить изменения?");
    switch (t) {
    case QMessageBox::Yes:
        query->prepare("UPDATE customer SET name = :name, email = :email, password = :password WHERE id = :id;");
        query->bindValue(":name", ui->nameLineEdit->text());
        query->bindValue(":email", ui->emailLineEdit->text());
        query->bindValue(":password", ui->newPasswordLineEdit->text());
        query->bindValue(":id", account.value(0).toString());
        if (!query->exec()){
            qDebug() << query->lastError();
        }
        logOutButton_clicked();
        break;
    case QMessageBox::No:
        break;
    }
}
