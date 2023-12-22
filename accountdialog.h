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
    explicit AccountDialog(QWidget *parent = nullptr, QSqlDatabase *db = nullptr, QSqlRecord *account = nullptr);
    ~AccountDialog();

signals:
    void logOutSignal();

private slots:
    void logOutButton_clicked();
    void saveButton_clicked();

private:
    Ui::AccountDialog *ui;
    QSqlQuery *query;
    QSqlDatabase db;
    QSqlRecord account;
};

#endif // ACCOUNTDIALOG_H
