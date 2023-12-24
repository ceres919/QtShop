#ifndef REGISTRATIONFORM_H
#define REGISTRATIONFORM_H

#include <QWidget>
#include <QMessageBox>
#include <QtSql>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlRelationalTableModel>
#include <QRegExp>
#include "mainwindow.h"

namespace Ui {
class RegistrationForm;
}

class RegistrationForm : public QWidget
{
    Q_OBJECT

public:
    explicit RegistrationForm(QWidget *parent = nullptr);
    ~RegistrationForm();
    void setDataBase(QSqlDatabase base);

private slots:
    void signUpButton_clicked();

private:
    Ui::RegistrationForm *ui;
    QSqlDatabase db;
    QSqlQuery *query;
    //MainWindow *mainWindow;
};

#endif // REGISTRATIONFORM_H
