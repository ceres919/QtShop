#ifndef ACCOUNTDIALOG_H
#define ACCOUNTDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QRegExp>
#include <QtSql>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlRelationalTableModel>

namespace Ui {
class AccountDialog;
}

class AccountDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AccountDialog(QWidget *parent = nullptr);
    ~AccountDialog();
    void setDataBase(QSqlDatabase base);
    void setAccount(QSqlRecord acc);

signals:
    void logOutSignal();

private slots:
    void logOutButton_clicked();
    void saveButton_clicked();

private:
    Ui::AccountDialog *ui;
    QSqlDatabase db;
    QSqlRecord account;
    QSqlQuery *query;

};

#endif // ACCOUNTDIALOG_H
