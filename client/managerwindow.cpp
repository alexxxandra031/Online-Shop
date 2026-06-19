#include "managerwindow.h"
#include "ui_managerwindow.h"

ManagerWindow::ManagerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ManagerWindow)
    , m_userId(-1)
{
    ui->setupUi(this);
    connect(ClientManager::getInstance(), &ClientManager::dataReceived, this, [this](const QString &data) {
        QStringList parts = data.split("|");
        QString command = parts.value(0);
        qDebug() << "[CLIENT] Received:" << data;
        if (command == "PRODUCTS_DATA") {
            QStandardItemModel *model = new QStandardItemModel(this);
            model->setHorizontalHeaderLabels({"id", "название", "цена", "ед.", "остаток", "категория"});

            if (parts.size() > 1 && !parts[1].isEmpty()) {
                QStringList rows = parts[1].split("#", Qt::SkipEmptyParts);
                for (const QString &r : rows) {
                    QList<QStandardItem*> items;
                    for (const QString &c : r.split(";")) items.append(new QStandardItem(c));
                    model->appendRow(items);
                }
            }
            ui->tableStock->setModel(model);
            ui->tableStock->horizontalHeader()->setStretchLastSection(true);
        }
        else if (command == "ALL_ORDERS_DATA") {
            QStandardItemModel *model = new QStandardItemModel(this);
            model->setHorizontalHeaderLabels({"id", "дата_продажи", "доставка", "сумма", "id_клиента"});

            if (parts.size() > 1 && !parts[1].isEmpty()) {
                QStringList rows = parts[1].split("#", Qt::SkipEmptyParts);
                for (const QString &r : rows) {
                    QList<QStandardItem*> items;
                    for (const QString &c : r.split(";")) items.append(new QStandardItem(c));
                    model->appendRow(items);
                }
            }
            ui->tableOrders->setModel(model);
            ui->tableOrders->horizontalHeader()->setStretchLastSection(true);
        }
        else if (command == "ALL_CLIENTS_DATA") {
            QStandardItemModel *model = new QStandardItemModel(this);
            model->setHorizontalHeaderLabels({"id", "фамилия", "имя", "email", "телефон", "статус"});

            if (parts.size() > 1 && !parts[1].isEmpty()) {
                QStringList rows = parts[1].split("#", Qt::SkipEmptyParts);
                for (const QString &r : rows) {
                    QList<QStandardItem*> items;
                    for (const QString &c : r.split(";")) items.append(new QStandardItem(c));
                    model->appendRow(items);
                }
            }
            ui->tableClients->setModel(model);
            ui->tableClients->horizontalHeader()->setStretchLastSection(true);
        }
        else if (command == "UPDATE_DELIVERY_OK") {
            QMessageBox::information(this, "успех", "дата доставки назначена");
            ClientManager::getInstance()->sendRequest("GET_ALL_ORDERS");
        }
    });

    connect(ui->btnStock, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageStock);
        ClientManager::getInstance()->sendRequest("GET_PRODUCTS");
    });

    connect(ui->btnOrders, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageOrders);
        ClientManager::getInstance()->sendRequest("GET_ALL_ORDERS");
    });

    connect(ui->btnClients, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageClients);
        ClientManager::getInstance()->sendRequest("GET_CLIENTS");
    });

    connect(ui->btnProfile, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageProfile);
    });



    connect(ui->btnSetDelivery, &QPushButton::clicked, this, [this]() {
        // для теста берем id заказа 1 (в идеале нужно брать выделенную строку таблицы)
        QString date = ui->dateDelivery->date().toString("dd.MM.yyyy");
        QModelIndex index = ui->tableOrders->currentIndex();
        if (!index.isValid()) {
            QMessageBox::warning(this, "Ошибка", "Выберите заказ");
            return;
        }
        int orderId = ui->tableOrders->model()
                          ->data(ui->tableOrders->model()->index(index.row(), 0)).toInt();
        QString req = QString("UPDATE_DELIVERY|%1|%2").arg(orderId).arg(date);

        ClientManager::getInstance()->sendRequest(req);
    });

    connect(ui->btnSaveProfile, &QPushButton::clicked, this, [this]() {
        QString email = ui->lineEmail->text();
        QString password = ui->linePassword->text();

        QString req = QString("UPDATE_PROFILE|%1|%2|%3|%4|%5|%6")
                          .arg(m_userId)
                          .arg("Сотрудник").arg("Менеджер")
                          .arg(email).arg("нет").arg(password);
        ClientManager::getInstance()->sendRequest(req);
    });
}

ManagerWindow::~ManagerWindow()
{
    delete ui;
}
