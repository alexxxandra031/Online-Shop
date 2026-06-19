#include "adminwindow.h"
#include "ui_adminwindow.h"

AdminWindow::AdminWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AdminWindow)
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
                for (const QString &r : parts[1].split("#", Qt::SkipEmptyParts)) {
                    QList<QStandardItem*> items;
                    for (const QString &c : r.split(";")) items.append(new QStandardItem(c));
                    model->appendRow(items);
                }
            }
            ui->tableCatalogCrud->setModel(model);
            ui->tableCatalogCrud->horizontalHeader()->setStretchLastSection(true);
        }
        else if (command == "ALL_CLIENTS_DATA") {
            QStandardItemModel *model = new QStandardItemModel(this);
            model->setHorizontalHeaderLabels({"id", "фамилия", "имя", "email", "телефон", "статус"});
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
        else if (command == "CATEGORIES_DATA") {
            QStandardItemModel *model = new QStandardItemModel(this);
            model->setHorizontalHeaderLabels({"id", "название"});
            if (parts.size() > 1 && !parts[1].isEmpty()) {
                for (const QString &r : parts[1].split("#", Qt::SkipEmptyParts)) {
                    QStringList cols = r.split(";");
                    if (cols.size() >= 2) {
                        QList<QStandardItem*> items;
                        items.append(new QStandardItem(cols[0])); // id
                        items.append(new QStandardItem(cols[1])); // name
                        model->appendRow(items);
                    }
                }
            }
            ui->tableCategories->setModel(model);
            ui->tableCategories->horizontalHeader()->setStretchLastSection(true);
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
        ClientManager::getInstance()->sendRequest("GET_CATEGORIES");
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

    connect(ui->btnAddProduct, &QPushButton::clicked, this, [this]() {
        // Открыть диалог добавления
        QString name = QInputDialog::getText(this, "Добавить", "Название:");
        if (name.isEmpty()) return;
        double price = QInputDialog::getDouble(this, "Добавить", "Цена:");
        int stock = QInputDialog::getInt(this, "Добавить", "Количество:");
        int catId = QInputDialog::getInt(this, "Добавить", "ID категории:");

        ClientManager::getInstance()->sendRequest(
            QString("ADD_PRODUCT|%1|%2|шт|%3|%4")
                .arg(name).arg(price).arg(stock).arg(catId)
            );
    });


    connect(ui->btnAddEmployee, &QPushButton::clicked, this, [this]() {
        QString email = QInputDialog::getText(this, "Добавить менеджера", "Email:");
        if (email.isEmpty()) return;
        QString password = QInputDialog::getText(this, "Добавить менеджера", "Пароль:", QLineEdit::Password);
        if (password.isEmpty()) return;

        ClientManager::getInstance()->sendRequest(
            QString("ADD_MANAGER|%1|%2").arg(email).arg(password)
            );
    });
    connect(ui->btnDelUser, &QPushButton::clicked, this, [this]() {
        QModelIndex index = ui->tableUsersCrud->currentIndex();
        if (!index.isValid()) {
            QMessageBox::warning(this, "Ошибка", "Выберите пользователя");
            return;
        }
        int userId = ui->tableUsersCrud->model()
                         ->data(ui->tableUsersCrud->model()->index(index.row(), 0)).toInt();

        ClientManager::getInstance()->sendRequest(
            QString("DELETE_USER|%1").arg(userId)
            );
    });

    connect(ui->btnAddDiscount, &QPushButton::clicked, this, [this]() {
        QString type = QInputDialog::getText(this, "Добавить скидку", "Тип:");
        if (type.isEmpty()) return;
        double minSum = QInputDialog::getDouble(this, "Добавить скидку", "Мин. сумма:");
        double percent = QInputDialog::getDouble(this, "Добавить скидку", "Процент:");

        ClientManager::getInstance()->sendRequest(
            QString("ADD_DISCOUNT|%1|%2|%3").arg(type).arg(minSum).arg(percent)
            );
    });


    connect(ui->btnSaveProfile, &QPushButton::clicked, this, [this]() {
        QString email = ui->lineEmail->text();
        QString password = ui->linePassword->text();

        ClientManager::getInstance()->sendRequest(
            QString("UPDATE_PROFILE|%1|%2|%3|%4|%5|%6")
                .arg(m_userId)
                .arg("Админ").arg("Главный")
                .arg(email).arg("нет").arg(password)
            );
    });

    connect(ui->btnEditProduct, &QPushButton::clicked, this, [this]() {

        QModelIndex index = ui->tableCatalogCrud->currentIndex();
        if (!index.isValid()) {
            QMessageBox::warning(this, "Ошибка", "Выберите товар для редактирования");
            return;
        }


        QAbstractItemModel *model = ui->tableCatalogCrud->model();
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


    connect(ui->btnAddCategory, &QPushButton::clicked, this, [this]() {
        QString name = QInputDialog::getText(this, "Добавить категорию", "Название:");
        if (name.isEmpty()) return;
        ClientManager::getInstance()->sendRequest(QString("ADD_CATEGORY|%1").arg(name));
    });


    connect(ui->btnEditCategory, &QPushButton::clicked, this, [this]() {
        QModelIndex index = ui->tableCategories->currentIndex();
        if (!index.isValid()) {
            QMessageBox::warning(this, "Ошибка", "Выберите категорию");
            return;
        }
        int catId = ui->tableCategories->model()
                        ->data(ui->tableCategories->model()->index(index.row(), 0)).toInt();
        QString oldName = ui->tableCategories->model()
                              ->data(ui->tableCategories->model()->index(index.row(), 1)).toString();
        QString newName = QInputDialog::getText(this, "Изменить категорию",
                                                "Новое название:", QLineEdit::Normal, oldName);
        if (newName.isEmpty()) return;
        ClientManager::getInstance()->sendRequest(
            QString("UPDATE_CATEGORY|%1|%2").arg(catId).arg(newName)
            );
    });




}

AdminWindow::~AdminWindow()
{
    delete ui;
}
