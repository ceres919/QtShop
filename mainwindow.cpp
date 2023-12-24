#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->netOrderPushButton->animateClick();
    ui->itemsListBodyTabWidget->tabBar()->hide();
    ui->tableView->horizontalHeader()->hide();
    ui->tableView->verticalHeader()->hide();
    ui->tableView->resizeColumnsToContents();
    ui->categoryComboBox->setVisible(false);
    ui->label->setVisible(false);

    query = new QSqlQuery(db);

    //смена типа и категории товаров
    connect(ui->catalogWayComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(productTypeComboBox_onChange(int)));
    connect(ui->categoryComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(category_onChange(int)));

    //кнопка лого
    connect(ui->logoPushButton, SIGNAL(clicked()), this, SLOT(logoButton_clicked()));

    //чтобы выйти из аккаунта
    accountWindow = new AccountDialog();
    connect(accountWindow, &AccountDialog::logOutSignal, this, &MainWindow::logOut_event);

    //вызов окон аккаунта и корзины
    connect(ui->accountPushButton, SIGNAL(clicked()), this, SLOT(accountButton_clicked()));
    connect(ui->cartPushButton, SIGNAL(clicked()), this, SLOT(cartButton_clicked()));


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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setDataBase(QSqlDatabase base){
    db = base;
    accountWindow->setDataBase(db);

    //таблица
    model = new QSqlRelationalTableModel (this, db);
    model->setJoinMode(QSqlRelationalTableModel::LeftJoin);
    model->setEditStrategy (QSqlRelationalTableModel::OnManualSubmit);
    model->setTable ("product");
    model->setRelation(model->fieldIndex("category_id"), QSqlRelation("type_category", "id", "name"));
    model->setRelation(model->fieldIndex("category_id_type"), QSqlRelation("product_type", "id", "name"));
    model->select();

    model->setHeaderData(4, Qt::Horizontal, tr("type"));
    ui->tableView->setModel(model);
    ui->tableView->hideColumn(0);
    ui->tableView->hideColumn(3);
    ui->tableView->hideColumn(4);

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //комбокс каталога
    if(!query->exec("SELECT name FROM product_type;")){
        qDebug() << query->lastError();
    }
    else{
        while(query->next()){
            ui->catalogWayComboBox->addItem(query->record().value(0).toString());
        }
    }
}

void MainWindow::setAccount(QSqlRecord acc){
    account = acc;
    accountWindow->setAccount(account);
}

void MainWindow::setCart_id(int cart)
{
    cart_id = cart;
}




void MainWindow::logoButton_clicked(){
    ui->mainTabWidget->setCurrentIndex(0);
}

void MainWindow::productTypeComboBox_onChange(int currentIndex){
    QString currentType = ui->catalogWayComboBox->currentText();
    if(currentType != "Все категории товаров"){
        model->setFilter("product_type_name_2 = '" + currentType +"'");
        newWay(currentIndex);
    } else{
        model->setFilter(QString());
        ui->label->setVisible(false);
        ui->categoryComboBox->setVisible(false);
    }
    ui->catalogWayComboBox->AdjustToContents;
    model->select();
}

void MainWindow::category_onChange(int currentIndex){
    QString currentCategory = ui->categoryComboBox->currentText();
    QString currentType = ui->catalogWayComboBox->currentText();
    QString typeFilter = "product_type_name_2 = '" + currentType + "'";
    QString categoryFilter = "type_category_name_3 = '" + currentCategory + "'";

    if(currentCategory != "Все"){
        model->setFilter(typeFilter + " AND " + categoryFilter);
    } else{
        model->setFilter(typeFilter);
    }
    ui->categoryComboBox->adjustSize();
    model->select();
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
    QString sort;
    if(checked == true){
        for(auto sortButton : sortButtonsGroup->findChildren<QRadioButton *>())
        {
            if(sortButton != QObject::sender()){
                sortButton->setChecked(false);
            }
            else{
                sort = sortButton->objectName();
            }
        }
        setSortingType(sort);
    } else{
        ui->tableView->setSortingEnabled(false);
        ui->tableView->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);
        ui->tableView->setSortingEnabled(true);
        ui->tableView->sortByColumn(0, Qt::AscendingOrder);
    }
}


void MainWindow::setSortingType(QString currentSort){
    ui->tableView->setSortingEnabled(false);
    ui->tableView->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->sortByColumn(0, Qt::AscendingOrder);

    if(currentSort == "sortByNameRadioButton"){
        ui->tableView->sortByColumn(1, Qt::AscendingOrder);
    } else if(currentSort == "sortByPriceIncRadioButton") {
        ui->tableView->sortByColumn(5, Qt::AscendingOrder);
    } else if(currentSort == "sortByPriceDecRadioButton"){
        ui->tableView->sortByColumn(5, Qt::DescendingOrder);
    }
}


void MainWindow::newWay(int typeIndex){
    ui->categoryComboBox->clear();
    query->prepare("SELECT name FROM type_category WHERE type_id = :type_id;");
    query->bindValue(":type_id", typeIndex);
    if(!query->exec()){
        qDebug() << query->lastError();
    }
    else{
        ui->categoryComboBox->addItem("Все");
        while(query->next()){
            ui->categoryComboBox->addItem(query->record().value(0).toString());
        }
    }
    ui->label->setVisible(true);
    ui->categoryComboBox->setVisible(true);
}

void MainWindow::accountButton_clicked(){
    accountWindow->setModal(true);
    accountWindow->exec();
}
void MainWindow::cartButton_clicked(){
    cartWindow = new CartDialog();
    cartWindow->setCart_id(cart_id);
    cartWindow->setModal(true);
    cartWindow->exec();
}

void MainWindow::logOut_event(){
    this->close();
    LogInForm *logWindow = new LogInForm();
    logWindow->show();
}
