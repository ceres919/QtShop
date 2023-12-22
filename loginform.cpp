#include "loginform.h"
#include "ui_loginform.h"

LogInForm::LogInForm(QWidget *parent, QSqlDatabase *db) :
    QWidget(parent),
    db(*db),
    ui(new Ui::LogInForm)
{
    ui->setupUi(this);
    query = new QSqlQuery(*db);

    connect(ui->logInPushButton, SIGNAL(clicked()), this, SLOT(logButton_clicked()));
    connect(ui->registrationPushButton, SIGNAL(clicked()), this, SLOT(regButton_clicked()));
    connect(ui->passwordLineEdit, SIGNAL(returnPressed()), this, SLOT(logButton_clicked()));
}

LogInForm::~LogInForm()
{
    delete ui;
}

void LogInForm::logButton_clicked(){
    QString email = ui->emailLineEdit->text();
    QString password = ui->passwordLineEdit->text();
    QSqlRecord account;
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

    hide();
    mainWindow = new MainWindow(nullptr, &db, &account);
    mainWindow->show();
}

void LogInForm::regButton_clicked(){
    hide();
    signUpWindow = new RegistrationForm(nullptr, &db);
    signUpWindow->show();
}
