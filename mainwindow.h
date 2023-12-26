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
#include <QSpinBox>
#include <QList>
#include <QListWidget>
#include <QTimer>
#include "accountdialog.h"
#include "loginform.h"
#include "cartdialog.h"
#include "basketdelegate.h"
#include "imagemanager.h"
#include "networking.h"

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

signals:
    void itemAddedToCart(const QVariant& data);

public slots:
    void logOut_event();
    void addToBasket(int itemId);

private slots:
    void accountButton_clicked();
    void cartButton_clicked();  
    void checkForSortButtonsExclusivity(bool checked);
    void switchOrderTabs();
    void logoButton_clicked();
    void productTypeComboBox_onChange(int currentIndex);
    void category_onChange(int currentIndex);
    void setPriceFilter();
    void searchLineEdit_textChanged(const QString &arg1);
    void searchButton_clicked();
    void loadImage(const QString &urlString);
    void setImage(const QUrl &imageUrl, const QImage &image);
    void resetImage(QString filter);
    void updateImages(const QUrl &imageUrl);
    void onImageRead(const QUrl &imageUrl, const QImage &image);
    void codeChangeTimer();
    void countdownCodeTimer();
    void addAddressButton_clicked();
    void deleteAddressButton_clicked();

private:
    void setMinMax(int min, int max);
    void setMinMaxSlider(QSlider *slider, int min, int max);
    void setMinMaxSpinBox(QSpinBox *spin, int min, int max);
    void newWay(int typeIndex);
    void setSortingType(QString currentSort);
    void minMaxPriceOfProducts();
    Ui::MainWindow *ui;
    QSettings *settings;
    QSqlDatabase db;
    QSqlRelationalTableModel *model;
    QSqlQuery *query;
    QSqlRecord account;
    int cart_id;
    QImage currentImage;
    QMap<QUrl, int> photo_map;
    AccountDialog *accountWindow;
    CartDialog *cartWindow;
    QGroupBox *sortButtonsGroup;
    QTimer *codeTimer;
    QTimer *timer;

    int time = 0;
};
#endif // MAINWINDOW_H
