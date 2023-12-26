#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include <QMessageBox>
#include <QtSql>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlRelationalTableModel>
#include <QSettings>

#include "registrationform.h"
#include "mainwindow.h"

namespace Ui {
class LogInForm;
}

class LogInForm : public QWidget
{
    Q_OBJECT

public:
    explicit LogInForm(QWidget *parent = nullptr);
    ~LogInForm();
    void setDataBase(QSqlDatabase base);

private slots:
    void regButton_clicked();
    void logButton_clicked();
    void rememberMe(int arg1);

private:
    Ui::LogInForm *ui;
    QSettings *settings;
    QSqlDatabase db;
    QSqlQuery *query;
    //MainWindow *mainWindow;
    //RegistrationForm *signUpWindow;

};

#endif // LOGINFORM_H
