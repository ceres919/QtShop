#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGroupBox>
#include "accountdialog.h"
#include "cartdialog.h"
#include <QtSql>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, QSqlDatabase *db = nullptr, QSqlRecord *account = nullptr);
    ~MainWindow();
public slots:
    void logOut_event();

private slots:
    void accountButton_clicked();
    void cartButton_clicked();
    void newWay();
    void checkForSortButtonsExclusivity(bool);
    void switchOrderTabs();
    void logoButton_clicked();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QSqlRecord account;
    AccountDialog *accountWindow;
    CartDialog *cartWindow;
    QGroupBox *sortButtonsGroup;
};
#endif // MAINWINDOW_H
