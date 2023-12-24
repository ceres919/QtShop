#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGroupBox>
#include <QtSql>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlRelationalTableModel>
#include <QSqlRelationalDelegate>
#include <QComboBox>
#include <QLayout>
#include <QRadioButton>
#include <QList>

#include "accountdialog.h"
#include "loginform.h"
#include "cartdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setDataBase(QSqlDatabase base);
    void setCart_id(int cart);
    void setAccount(QSqlRecord acc);

public slots:
    void logOut_event();

private slots:
    void accountButton_clicked();
    void cartButton_clicked();
    void newWay(int typeIndex);
    void checkForSortButtonsExclusivity(bool checked);
    void switchOrderTabs();
    void logoButton_clicked();
    void productTypeComboBox_onChange(int currentIndex);
    void category_onChange(int currentIndex);
    void setSortingType(QString currentSort);

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QSqlRelationalTableModel *model;
    QSqlQuery *query;
    QSqlRecord account;
    int cart_id;
    AccountDialog *accountWindow;
    CartDialog *cartWindow;
    QGroupBox *sortButtonsGroup;
};
#endif // MAINWINDOW_H
