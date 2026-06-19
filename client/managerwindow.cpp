#include "managerwindow.h"
#include "ui_managerwindow.h"

ManagerWindow::ManagerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ManagerWindow)
    , m_userId(-1)
{
    ui->setupUi(this);
    connect(ClientManager::getInstance(), &ClientManager::dataReceived, this, [this](const QString &rawData) {
        QString data = rawData.trimmed();

        QStringList parts = data.split("|");
        QString command = parts.value(0).trimmed();

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
            model->setHorizontalHeaderLabels({"id", "название", "цена", "ед.", "остаток", "категория"});

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
        else if (command == "UPDATE_PROFILE_OK") {
            QMessageBox::information(this, "Успех", parts.value(1));
        }
        else if (command == "UPDATE_PROFILE_FAIL") {
            QMessageBox::warning(this, "Ошибка", parts.value(1));
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

    connect(ui->btnChangePassword, &QPushButton::clicked, this, [this]() {
        QString newPassword = ui->lineNewPassword->text();
        if (newPassword.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Введите новый пароль");
            return;
        }
        // Отправляем команду UPDATE_PROFILE с пустыми полями (только пароль)
        ClientManager::getInstance()->sendRequest(
            QString("UPDATE_PROFILE|||||%1").arg(newPassword)
            );
    });
    connect(ui->btnEditProduct, &QPushButton::clicked, this, [this]() {
        QModelIndex index = ui->tableStock->currentIndex();
        if (!index.isValid()) {
            QMessageBox::warning(this, "Ошибка", "Выберите товар для редактирования");
            return;
        }

        QAbstractItemModel *model = ui->tableStock->model();
        int row = index.row();

        int id = model->data(model->index(row, 0)).toInt();
        QString oldName = model->data(model->index(row, 1)).toString();
        double oldPrice = model->data(model->index(row, 2)).toDouble();
        QString oldUnit = model->data(model->index(row, 3)).toString();
        int oldStock = model->data(model->index(row, 4)).toInt();
        int oldCategory = model->data(model->index(row, 5)).toInt();


        QString name = QInputDialog::getText(this, "Изменить товар",
                                             "Название:", QLineEdit::Normal, oldName);
        if (name.isEmpty()) return;


        double price = QInputDialog::getDouble(this, "Изменить товар",
                                               "Цена:", oldPrice, 0, 9999999, 2);

        QStringList units = {"шт", "кг", "л", "мм", "г", "мл", "м"};
        bool ok;
        QString unit = QInputDialog::getItem(this, "Изменить товар", "Единица измерения:",
                                             units, units.indexOf(oldUnit), false, &ok);
        if (!ok) return;


        int stock = QInputDialog::getInt(this, "Изменить товар",
                                         "Количество на складе:", oldStock, 0, 9999999);


        int category = QInputDialog::getInt(this, "Изменить товар",
                                            "ID категории:", oldCategory, 1, 9999999);


        QString req = QString("UPDATE_PRODUCT|%1|%2|%3|%4|%5|%6")
                          .arg(id)
                          .arg(name)
                          .arg(price)
                          .arg(unit)
                          .arg(stock)
                          .arg(category);

        ClientManager::getInstance()->sendRequest(req);
    });
}

ManagerWindow::~ManagerWindow()
{
    delete ui;
}
