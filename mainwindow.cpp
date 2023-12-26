#include "mainwindow.h"
#include "./ui_mainwindow.h"

#define I2P(image) QPixmap::fromImage(image)

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    query = new QSqlQuery(db);
    resetImage(QString::number(1));

    ui->setupUi(this);
    ui->listOrderPushButton->animateClick();
    ui->itemsListBodyTabWidget->tabBar()->hide();
    ui->tableView->horizontalHeader()->hide();
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->resizeColumnsToContents();
    ui->categoryComboBox->setVisible(false);
    ui->label->setVisible(false);

    settings = new QSettings("Qt_Shop", "DeliverySettings", this);

    //поиск
    connect(ui->searchLineEdit, SIGNAL(textChanged(QString)), this, SLOT(searchLineEdit_textChanged(QString)));
    connect(ui->searchLineEdit, SIGNAL(returnPressed()), this, SLOT(searchButton_clicked()));

    //смена типа и категории товаров
    connect(ui->catalogWayComboBox, SIGNAL(activated(int)), this, SLOT(productTypeComboBox_onChange(int)));
    connect(ui->categoryComboBox, SIGNAL(activated(int)), this, SLOT(category_onChange(int)));

    //кнопка лого
    connect(ui->logoPushButton, SIGNAL(clicked()), this, SLOT(logoButton_clicked()));

    //чтобы выйти из аккаунта
    accountWindow = new AccountDialog();
    connect(accountWindow, &AccountDialog::logOutSignal, this, &MainWindow::logOut_event);
    cartWindow = new CartDialog();

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

    connect(ui->minPriceSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setPriceFilter()));
    connect(ui->maxPriceSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setPriceFilter()));

    //проверка на эксклюзивность выбранного варианта сортировки
    sortButtonsGroup = ui->sortButtonsGroupBox;
    for(auto sortButton : sortButtonsGroup->children())
    {
        connect(sortButton, SIGNAL(toggled(bool)), this, SLOT(checkForSortButtonsExclusivity(bool)));
    }

    //Таб Доставка ////////////////

    //таймер для кода
    time = 30;
    codeChangeTimer();
    countdownCodeTimer();

    codeTimer = new QTimer (this);
    timer = new QTimer (this);
    connect(codeTimer, SIGNAL(timeout()), this, SLOT(codeChangeTimer()));
    connect(timer, SIGNAL(timeout()), this, SLOT(countdownCodeTimer()));

    codeTimer->start(30000);
    timer->start(1000);

    //адреса доставки
    connect(ui->addAddressPushButton, SIGNAL(clicked()), this, SLOT(addAddressButton_clicked()));
    connect(ui->addressLineEdit, SIGNAL(returnPressed()), this, SLOT(addAddressButton_clicked()));

    connect(ui->deleteAddressPushButton, SIGNAL(clicked()), this, SLOT(deleteAddressButton_clicked()));


    if (!settings->value("DeliverySettings/address").toStringList().isEmpty()) {

        for(auto it : settings->value("DeliverySettings/address").toStringList()){
            QListWidgetItem *item = new QListWidgetItem(ui->addressesList);
            ui->addressesList->setItemWidget(item, new QRadioButton(it));
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Основное окно ///////////////////////////////

void MainWindow::setCart_id(int cart)
{
    cart_id = cart;
}

void MainWindow::logoButton_clicked(){
    ui->mainTabWidget->setCurrentIndex(0);
}

void MainWindow::searchButton_clicked()
{
    ui->mainTabWidget->setCurrentIndex(1);
}

void MainWindow::setDataBase(QSqlDatabase base){
    db = base;
    accountWindow->setDataBase(db);

    //таблица
    model = new QSqlRelationalTableModel (this, db);
    model->setJoinMode(QSqlRelationalTableModel::LeftJoin);
    model->setEditStrategy (QSqlRelationalTableModel::OnManualSubmit);
    model->setTable ("product");
    model->setFilter(QString::number(1));
    model->setRelation(model->fieldIndex("category_id"), QSqlRelation("type_category", "id", "name"));
    model->setRelation(model->fieldIndex("category_id_type"), QSqlRelation("product_type", "id", "name"));
    model->select();

    ui->tableView->setModel(model);
    ui->tableView->hideColumn(3);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //кнопка "добавить в корзину"
    BasketButtonDelegate *delegate = new BasketButtonDelegate(ui->tableView);
    ui->tableView->setItemDelegateForColumn(4, delegate);
    connect(delegate, &BasketButtonDelegate::addToBasketClicked, this, &MainWindow::addToBasket);

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
    query->prepare("SELECT * FROM cart WHERE customer_id = :customer_id;");
    query->bindValue(":customer_id", account.value(0));
    if (!query->exec()){
        qDebug() << query->lastError();
    } else {
        if (query->next())
            cart_id = query->record().value(0).toInt();
    }
//    cartWindow = new CartDialog();
    cartWindow->setCart_id(cart_id);
}

void MainWindow::accountButton_clicked(){
    accountWindow->setModal(true);
    accountWindow->exec();
}

void MainWindow::cartButton_clicked(){
    disconnect(this, &MainWindow::itemAddedToCart, cartWindow, &CartDialog::onItemAddedToCart);
    connect(this, &MainWindow::itemAddedToCart, cartWindow, &CartDialog::onItemAddedToCart);
    //cartWindow->setDeliveryAddress(ui->addressGroupBox->);

    for(int i = 0; i < ui->addressesList->count(); ++i){
        auto item = ui->addressesList->item(i);
        QRadioButton *qbutton = qobject_cast<QRadioButton*>(ui->addressesList->itemWidget(item));
        if(qbutton->isChecked()){
            cartWindow->setDeliveryAddress(qbutton->text());
        }
    }

    cartWindow->updateCart();
    cartWindow->setModal(true);
    cartWindow->exec();
}

void MainWindow::logOut_event(){
    this->close();
    LogInForm *logWindow = new LogInForm();
    logWindow->show();
}


// Таб Каталог ///////////////////////////////////////////

void MainWindow::switchOrderTabs(){
    if(QObject::sender()->objectName() == "netOrderPushButton"){
        ui->itemsListBodyTabWidget->setCurrentIndex(0);
    }
    else{
        ui->itemsListBodyTabWidget->setCurrentIndex(1);
    }
}

void MainWindow::setPriceFilter(){
    QString filter = "price BETWEEN " + QString::number(ui->minPriceSpinBox->value()) + " AND " + QString::number(ui->maxPriceSpinBox->value());
    model->setFilter(filter);
    resetImage("(" + model->filter() + ")");
}

void MainWindow::productTypeComboBox_onChange(int currentIndex){
    QString currentType = ui->catalogWayComboBox->currentText();
    if(currentType != "Все категории товаров"){
        model->setFilter("product_type_name_2 = '" + currentType +"'");       
        newWay(currentIndex);
    } else{
        model->setFilter(QString::number(1));
        ui->label->setVisible(false);
        ui->categoryComboBox->setVisible(false);
    }
    ui->catalogWayComboBox->adjustSize();
    resetImage("(" + model->filter() + ")");
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
    resetImage("(" + model->filter() + ")");
}

void MainWindow::newWay(int typeIndex){
    ui->categoryComboBox->clear();
    ui->label->setVisible(true);
    ui->categoryComboBox->setVisible(true);
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
        resetImage("(" + model->filter() + ")");
    }
}

void MainWindow::setSortingType(QString currentSort){
    ui->tableView->setSortingEnabled(false);
    ui->tableView->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->sortByColumn(0, Qt::AscendingOrder);

    if(currentSort == "sortByNameRadioButton"){
        ui->tableView->sortByColumn(1, Qt::AscendingOrder);
        resetImage("(" + model->filter() + ") ORDER BY product.name ASC");
    } else if(currentSort == "sortByPriceIncRadioButton") {
        ui->tableView->sortByColumn(5, Qt::AscendingOrder);
        resetImage("(" + model->filter() + ") ORDER BY product.price ASC");
    } else if(currentSort == "sortByPriceDecRadioButton"){
        ui->tableView->sortByColumn(5, Qt::DescendingOrder);
        resetImage("(" + model->filter() + ") ORDER BY product.price DESC");
    }
}

void MainWindow::setMinMaxSpinBox(QSpinBox *spin, int min, int max)
{
    spin->setMinimum(min);
    spin->setMaximum(max);
}

void MainWindow::setMinMaxSlider(QSlider *slider, int min, int max)
{
    slider->setMinimum(min);
    slider->setMaximum(max);
}

void MainWindow::addToBasket(int itemId)
{
    QModelIndex index = model->index(itemId, 0);
    QVariant data = model->data(index);
    //qDebug() << "Значение по 0 столбцу в строке с индексом" << itemId << ":" << data;
    emit itemAddedToCart(data);
}

void MainWindow::searchLineEdit_textChanged(const QString &arg1)
{
    QString filterLower = "LOWER(product.name) LIKE '%" + arg1.mid(1, arg1.size()-1).toLower() + "%'";
    QString filterUpper = "product.name LIKE '%" + arg1.toUpper() + "%'";
    QString searchInput = filterLower + " OR " + filterUpper;
    if (!arg1.isEmpty()) {
        model->setFilter(searchInput);
        resetImage("(" + searchInput + ")");
    }
    else{
        model->setFilter(QString::number(1));
        resetImage("(" + model->filter() + ")");
    }
}

void MainWindow::loadImage(const QString &urlString) {
    Networking::httpGetImageAsync(QUrl(urlString), this, "onImageRead");
}

void MainWindow::setImage(const QUrl &imageUrl, const QImage &image) {
    currentImage = ImageManager::normallyResized(image, 120);
    updateImages(imageUrl);
}

void MainWindow::resetImage(QString filter) {
    photo_map.clear();
    query->exec("SELECT product_photo.id, url, product_type.name as product_type_name_2, "
                "type_category.name as type_category_name_3 from product_photo INNER JOIN "
                "product ON product_photo.product_id = product.id LEFT JOIN type_category ON "
                "category_id=type_category.id LEFT JOIN product_type ON "
                "category_id_type=product_type.id WHERE " + filter + ";");
    int ind = 0;
    while(query->next()) {
        photo_map.insert(query->record().value(1).toString(), ind++);
        loadImage(query->record().value(1).toString());
    }
}

void MainWindow::updateImages(const QUrl &imageUrl) {
    if (photo_map.contains(imageUrl)) {
        QLabel *imageLabel = new QLabel();
        imageLabel->setPixmap(I2P(currentImage));
        ui->tableView->setIndexWidget(model->index(photo_map.value(imageUrl), 0), imageLabel);
        ui->tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    }
}

void MainWindow::onImageRead(const QUrl &imageUrl, const QImage &image) {
    Q_UNUSED(imageUrl)
    setImage(imageUrl, image);
}




// Таб Доставка ///////////////////////////////

void MainWindow::codeChangeTimer()
{
    QString codeNumder = "";
    for(int i = 0; i < 4; ++i){
        codeNumder += QString::number(rand() % ((9 + 1) - 0) + 0);
    }
    ui->codeLabel->setText(codeNumder);
    time = 30;
}

void MainWindow::countdownCodeTimer()
{
    QString text = "Код обновится через " + QString::number(time) + " секунд";
    ui->codeInfo->setText(text);
    time--;
}

void MainWindow::addAddressButton_clicked()
{
    if (!ui->addressLineEdit->text().isEmpty()) {
        QStringList addressList = settings->value("DeliverySettings/address").toStringList();
        addressList.append(ui->addressLineEdit->text());
        settings->setValue("DeliverySettings/address", addressList);
        QListWidgetItem *item = new QListWidgetItem(ui->addressesList);
        ui->addressesList->setItemWidget(item, new QRadioButton(ui->addressLineEdit->text()));
        ui->addressLineEdit->clear();
    }
}

void MainWindow::deleteAddressButton_clicked()
{
    ui->addressesList->takeItem(ui->addressesList->row(ui->addressesList->currentItem()));
    QStringList addressList;
    for(int i = 0; i < ui->addressesList->count(); ++i){
        auto item = ui->addressesList->item(i);
        QRadioButton *qbutton = qobject_cast<QRadioButton*>(ui->addressesList->itemWidget(item));
        addressList.append(qbutton->text());
    }
    settings->setValue("DeliverySettings/address", addressList);
}

void MainWindow::deliveryAddressChange(bool state)
{
    qDebug() << "нажато";
}


// Таб Главная ////////////////////////////////
