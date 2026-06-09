#include "adminwindow.h"
#include "ui_adminwindow.h"

AdminWindow::AdminWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AdminWindow)
{
    ui->setupUi(this);


    connect(ClientManager::getInstance(), &ClientManager::dataReceived, this, [this](const QString &data) {
        QStringList parts = data.split("|");
        QString command = parts.value(0);

        if (command == "PRODUCTS_DATA") {
            QStandardItemModel *model = new QStandardItemModel(this);
            model->setHorizontalHeaderLabels({"id", "название", "цена", "остаток"});
            if (parts.size() > 1 && !parts[1].isEmpty()) {
                for (const QString &r : parts[1].split("#", Qt::SkipEmptyParts)) {
                    QList<QStandardItem*> items;
                    for (const QString &c : r.split(";")) items.append(new QStandardItem(c));
                    model->appendRow(items);
                }
            }
            ui->tableCatalogCrud->setModel(model);
            ui->tableCatalogCrud->horizontalHeader()->setStretchLastSection(true);
        }
        else if (command == "CLIENTS_DATA") {
            QStandardItemModel *model = new QStandardItemModel(this);
            model->setHorizontalHeaderLabels({"id", "фамилия", "имя", "email", "телефон"});
            if (parts.size() > 1 && !parts[1].isEmpty()) {
                for (const QString &r : parts[1].split("#", Qt::SkipEmptyParts)) {
                    QList<QStandardItem*> items;
                    for (const QString &c : r.split(";")) items.append(new QStandardItem(c));
                    model->appendRow(items);
                }
            }
            ui->tableUsersCrud->setModel(model);
            ui->tableUsersCrud->horizontalHeader()->setStretchLastSection(true);
        }
        else if (command == "DISCOUNTS_DATA") {
            QStandardItemModel *model = new QStandardItemModel(this);
            model->setHorizontalHeaderLabels({"id", "тип", "мин_сумма", "процент"});
            if (parts.size() > 1 && !parts[1].isEmpty()) {
                for (const QString &r : parts[1].split("#", Qt::SkipEmptyParts)) {
                    QList<QStandardItem*> items;
                    for (const QString &c : r.split(";")) items.append(new QStandardItem(c));
                    model->appendRow(items);
                }
            }
            ui->tableDiscountsCrud->setModel(model);
            ui->tableDiscountsCrud->horizontalHeader()->setStretchLastSection(true);
        }
        else if (command == "ALL_ORDERS_DATA") {
            QStandardItemModel *model = new QStandardItemModel(this);
            model->setHorizontalHeaderLabels({"id", "дата", "доставка", "сумма", "id_клиента"});
            if (parts.size() > 1 && !parts[1].isEmpty()) {
                for (const QString &r : parts[1].split("#", Qt::SkipEmptyParts)) {
                    QList<QStandardItem*> items;
                    for (const QString &c : r.split(";")) items.append(new QStandardItem(c));
                    model->appendRow(items);
                }
            }
            ui->tableArchive->setModel(model);
            ui->tableArchive->horizontalHeader()->setStretchLastSection(true);
        }
        else if (command.endsWith("_OK")) {
            QMessageBox::information(this, "успех", "операция выполнена успешно");
        }
        else if (command.endsWith("_FAIL")) {
            QMessageBox::warning(this, "ошибка", parts.value(1));
        }
    });

    connect(ui->btnCatalogCrud, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageCatalogCrud);
        ClientManager::getInstance()->sendRequest("GET_PRODUCTS");
    });

    connect(ui->btnUsersCrud, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageUsersCrud);
        ClientManager::getInstance()->sendRequest("GET_CLIENTS");
    });

    connect(ui->btnDiscountsCrud, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageDiscountsCrud);
        ClientManager::getInstance()->sendRequest("GET_DISCOUNTS");
    });

    connect(ui->btnArchive, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageArchive);
        ClientManager::getInstance()->sendRequest("GET_ALL_ORDERS");
    });

    connect(ui->btnProfile, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageProfile);
    });

    //ЗАГЛУШКИ
    connect(ui->btnAddProduct, &QPushButton::clicked, this, [this]() {

        ClientManager::getInstance()->sendRequest("ADD_PRODUCT|Новый товар|1000|шт|50|1");
    });
    connect(ui->btnDelProduct, &QPushButton::clicked, this, [this]() {
        ClientManager::getInstance()->sendRequest("DELETE_PRODUCT|1");
    });

    connect(ui->btnAddEmployee, &QPushButton::clicked, this, [this]() {
        ClientManager::getInstance()->sendRequest("ADD_MANAGER|new_manager@test.com|12345");
    });
    connect(ui->btnDelUser, &QPushButton::clicked, this, [this]() {
        ClientManager::getInstance()->sendRequest("DELETE_CLIENT|2");
    });


    connect(ui->btnAddDiscount, &QPushButton::clicked, this, [this]() {
        ClientManager::getInstance()->sendRequest("ADD_DISCOUNT|Праздничная|10000|15");
    });
    connect(ui->btnDelDiscount, &QPushButton::clicked, this, [this]() {
        ClientManager::getInstance()->sendRequest("DELETE_DISCOUNT|1");
    });

    connect(ui->btnSaveProfile, &QPushButton::clicked, this, [this]() {
        QString email = ui->lineEmail->text();
        QString password = ui->linePassword->text();
        //ЗАГЛУШКА
        QString req = QString("UPDATE_PROFILE|1|Админ|Главный|%1|нет|%2").arg(email).arg(password);
        ClientManager::getInstance()->sendRequest(req);
    });
}

AdminWindow::~AdminWindow()
{
    delete ui;
}
