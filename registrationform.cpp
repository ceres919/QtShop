#include "registrationform.h"
#include "ui_registrationform.h"

RegistrationForm::RegistrationForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegistrationForm)
{
    ui->setupUi(this);
    query = new QSqlQuery(db);
    connect(ui->signUpPushButton, SIGNAL(clicked()), this, SLOT(signUpButton_clicked()));
}

RegistrationForm::~RegistrationForm()
{
    delete ui;
}

void RegistrationForm::setDataBase(QSqlDatabase base){
    db = base;
}

void RegistrationForm::signUpButton_clicked(){
    QRegExp name("^[А-ЯЁ][а-яё]+(?:\\s+[А-ЯЁ][а-яё]+){0,2}(?:\\s+[А-ЯЁ]\\.)?(?:\\s+[А-ЯЁ]\\.)?$");
    QRegExp email("^((?!\\.)[\\w\\-_.]*[^.])(@\\w+)(\\.\\w+(\\.\\w+)?[^.\\W])$");
    QRegExp password("^[A-Za-z\\d]{8,}");
    if(!password.exactMatch(ui->passwordLineEdit->text()) || !name.exactMatch(ui->nameLineEdit->text())
        || !email.exactMatch(ui->emailLineEdit->text())){
        QMessageBox::critical(this, "Ошибка", "Проверьте правильность ввода имени, пароля или email."
                                              "Пароль должен состоять минимум из 8 символов.");
        return;
    }

    //добавление аккаунта в таблицу
    query->prepare("INSERT INTO customer (name, email, password) VALUES (:name, :email, :password);");
    query->bindValue(":name", ui->nameLineEdit->text());
    query->bindValue(":email", ui->emailLineEdit->text());
    query->bindValue(":password", ui->passwordLineEdit->text());
    if (!query->exec()){
        qDebug() << query->lastError();
        QMessageBox::critical(this, "Ошибка", "Неудалось создать аккаунт");
        return ;
    }

    //получение аккаунта из таблицы
    QSqlRecord account;
    query->prepare("SELECT * FROM customer WHERE email = :email;");
    query->bindValue(":email", ui->emailLineEdit->text());
    if (!query->exec()){
        qDebug() << query->lastError();
        //Ошибка аккаунт не найден
    } else {
        if (query->next())
            account = query->record();
    }

    //создаём корзину для аккаунт
    int account_id = account.value(0).toInt();
    if(account_id != 0){
        query->prepare("INSERT INTO cart (customer_id) VALUES (:customer_id);");
        query->bindValue(":customer_id", account_id);
        if (!query->exec()){
            qDebug() << query->lastError();
            QMessageBox::critical(this, "Ошибка", "Неудалось создать аккаунт");
            return;
        }
    }
    else{
        QMessageBox::critical(this, "Ошибка", "Неудалось создать аккаунт");
        return;
    }

    int cart_id = 0;
    query->prepare("SELECT * FROM cart WHERE customer_id = :customer_id;");
    query->bindValue(":customer_id", account_id);
    if (!query->exec()){
        qDebug() << query->lastError();
    } else {
        if (query->next())
            cart_id = query->record().value(0).toInt();
    }

    hide();
    MainWindow *mainWindow = new MainWindow();
    mainWindow->setDataBase(db);
    mainWindow->setAccount(account);
    mainWindow->setCart_id(cart_id);
    mainWindow->show();
}
