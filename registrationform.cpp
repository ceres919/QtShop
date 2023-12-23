#include "registrationform.h"
#include "ui_registrationform.h"

RegistrationForm::RegistrationForm(QWidget *parent, QSqlDatabase *db) :
    QWidget(parent),
    db(*db),
    ui(new Ui::RegistrationForm)
{
    ui->setupUi(this);
    query = new QSqlQuery(*db);
    connect(ui->signUpPushButton, SIGNAL(clicked()), this, SLOT(signUpButton_clicked()));
}

RegistrationForm::~RegistrationForm()
{
    delete ui;
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

    query->prepare("INSERT INTO customer (name, email, password) VALUES (:name, :email, :password);");
    query->bindValue(":name", ui->nameLineEdit->text());
    query->bindValue(":email", ui->emailLineEdit->text());
    query->bindValue(":password", ui->passwordLineEdit->text());

    if (!query->exec()){
        qDebug() << query->lastError();
    }

    query->prepare("SELECT id FROM customer WHERE email = :email;");
    query->bindValue(":email", ui->emailLineEdit->text());
    if (!query->exec()){
        qDebug() << query->lastError();
    }
    int account_id = 0;
    if (query->next()){
        account_id = query->record().value(0).toInt();
    }

    if(account_id != 0){
        query->prepare("INSERT INTO cart (customer_id) VALUES (:customer_id);");
        query->bindValue(":customer_id", account_id);

        if (!query->exec()){
            qDebug() << query->lastError();
        }
    }
    else{
        QMessageBox::critical(this, "Ошибка", "Неудалось создать аккаунт");
        return;
    }


    hide();
    mainWindow = new MainWindow();
    mainWindow->show();


//    QString query = "SELECT * FROM customer";
//    QSqlQuery sqlquery (db);
//    if(!sqlquery.exec(query)) {
//        qDebug() << "query failed...";
//        //return;
//    }
//    while (sqlquery.next()){
//        qDebug() << sqlquery.value(1).toString();
//        //получим значение и например добавим его в комбобокс со всеми значениями

//    }
}
