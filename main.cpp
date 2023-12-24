#include "mainwindow.h"
#include "loginform.h"
#include <QApplication>
#include <QFile>
#include <QtSql>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlRelationalTableModel>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile file(":/resources/style/styles.css");
    file.open(QFile::ReadOnly);
    a.setStyleSheet(file.readAll());
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("/Users/yanakosteva/Univer/5_semester/VisProg/kurs_work/QtShop/shop.db");
    db.open();
    //MainWindow w;
    LogInForm w;
    w.setDataBase(db);
    w.show();
    return a.exec();
}
