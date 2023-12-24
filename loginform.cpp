#include "loginform.h"
#include "ui_loginform.h"

LogInForm::LogInForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogInForm)
{
    ui->setupUi(this);
    query = new QSqlQuery(db);

    connect(ui->logInPushButton, SIGNAL(clicked()), this, SLOT(logButton_clicked()));
    connect(ui->registrationPushButton, SIGNAL(clicked()), this, SLOT(regButton_clicked()));
    connect(ui->passwordLineEdit, SIGNAL(returnPressed()), this, SLOT(logButton_clicked()));
}

LogInForm::~LogInForm()
{
    delete ui;
}

void LogInForm::setDataBase(QSqlDatabase base){
    db = base;
}

void LogInForm::logButton_clicked(){
    QString email = ui->emailLineEdit->text();
    QString password = ui->passwordLineEdit->text();
    QSqlRecord account;

    //получение аккаунта из таблицы
    query->prepare("SELECT * from customer WHERE email = :email AND password = :password;");
    query->bindValue(":email", email);
    query->bindValue(":password", password);
    if (query->exec()){
        if (query->next()){
            account = query->record();
        } else {
            QMessageBox::critical(this, "Ошибка авторизации", "Проверьте правильность ввода email или пароля.");
            return;
        }
    } 

    //получение id корзины
    int account_id = account.value(0).toInt();
    int cart_id = 0;
    query->prepare("SELECT * FROM cart WHERE customer_id = :customer_id;");
    query->bindValue(":customer_id", account_id);
    if (!query->exec()){
        qDebug() << query->lastError();
        //Ошибка корзина не найдена
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

void LogInForm::regButton_clicked(){
    hide();
    RegistrationForm *signUpWindow = new RegistrationForm();
    signUpWindow->setDataBase(db);
    signUpWindow->show();
}
