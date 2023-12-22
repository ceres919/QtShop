#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "loginform.h"
//#include "QPushButton"
#include "QComboBox"
#include "QLayout"
#include "QRadioButton"
//#include "QWidget"



MainWindow::MainWindow(QWidget *parent, QSqlDatabase *db, QSqlRecord *account)
    : QMainWindow(parent),
    db(*db),
    account(*account),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->netOrderPushButton->animateClick();
    ui->itemsListBodyTabWidget->tabBar()->hide();


    //кнопка лого
    connect(ui->logoPushButton, SIGNAL(clicked()), this, SLOT(logoButton_clicked()));

    //чтобы выйти из аккаунта
    accountWindow = new AccountDialog(nullptr, db, account);
    connect(accountWindow, &AccountDialog::logOutSignal, this, &MainWindow::logOut_event);

    //вызов окон аккаунта и корзины
    connect(ui->accountPushButton, SIGNAL(clicked()), this, SLOT(accountButton_clicked()));
    connect(ui->cartPushButton, SIGNAL(clicked()), this, SLOT(cartButton_clicked()));

    //  !Доработать!  углубление пути
    connect(ui->newPushButton, SIGNAL(clicked()), this, SLOT(newWay()));

    //смена на сетку или список в каталоге
    connect(ui->netOrderPushButton, SIGNAL(clicked()), this, SLOT(switchOrderTabs()));
    connect(ui->listOrderPushButton, SIGNAL(clicked()), this, SLOT(switchOrderTabs()));

    //связка слайдера и спинбокса цены в фильтрах
    connect(ui->minPriceSpinBox, SIGNAL(valueChanged(int)), ui->minPriceSlider, SLOT(setValue(int)));
    connect(ui->minPriceSlider, SIGNAL(valueChanged(int)), ui->minPriceSpinBox, SLOT(setValue(int)));
    connect(ui->maxPriceSpinBox, SIGNAL(valueChanged(int)), ui->maxPriceSlider, SLOT(setValue(int)));
    connect(ui->maxPriceSlider, SIGNAL(valueChanged(int)), ui->maxPriceSpinBox, SLOT(setValue(int)));

    //проверка на эксклюзивность выбранного варианта сортировки
    sortButtonsGroup = ui->sortButtonsGroupBox;
    for(auto sortButton : sortButtonsGroup->children())
    {
        connect(sortButton, SIGNAL(toggled(bool)), this, SLOT(checkForSortButtonsExclusivity(bool)));
    }
//    QWidgetList *sortButtonsList;
//    QRadioButton *sortPriceincButton = ui->sortByPriceIncRadioButton;
//    sortButtonsList->append(sortPriceincButton);
//    QRadioButton *sortPriceDecButton = ui->sortByPriceDecRadioButton;
//    sortButtonsList->append(sortPriceDecButton);
//    QRadioButton *sortNameButton = ui->sortByPriceDecRadioButton;
//    sortButtonsList->append(sortNameButton);

//    for(auto button: sortButtonsList){
//        connect(button, SIGNAL(toggle()), this, SLOT(checkForSortButtonsExclusivity(bool)));
//    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::logoButton_clicked(){
    ui->mainTabWidget->setCurrentIndex(0);
}

void MainWindow::switchOrderTabs(){
    if(QObject::sender()->objectName() == "netOrderPushButton"){
        ui->itemsListBodyTabWidget->setCurrentIndex(0);
    }
    else{
        ui->itemsListBodyTabWidget->setCurrentIndex(1);
    }
}

void MainWindow::checkForSortButtonsExclusivity(bool checked){

    if(checked == true){
        // !!!!!вызвать функцию упорядычевания товароы!!!!!
        for(auto sortButton : sortButtonsGroup->findChildren<QRadioButton *>())
        {
            if(sortButton != QObject::sender()){
                sortButton->setChecked(false);
            }
        }
    }
}

void MainWindow::newWay(){
    QComboBox *comboBox = new QComboBox();
    comboBox->insertItem(0, "hi!");
    comboBox->insertItem(1, "hi!1");
    ui->catalogWayComboBox->insertItem(0, "blabla blabakabakjhd");
    //button2->setText("hi!");
    QLayout *wayWidget = ui->catalogWayWidget->layout();
    wayWidget->addWidget(comboBox);
    //ui->catalogWayWidge
}

void MainWindow::accountButton_clicked(){
    accountWindow->setModal(true);
    accountWindow->exec();
}
void MainWindow::cartButton_clicked(){
    cartWindow  = new CartDialog();
    cartWindow->setModal(true);
    cartWindow->exec();
}

void MainWindow::logOut_event(){
    this->close();
    LogInForm *logWindow = new LogInForm(nullptr, &db);
    logWindow->show();
}
