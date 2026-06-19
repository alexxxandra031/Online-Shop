#include "clientwindow.h"
#include "ui_clientwindow.h"

ClientWindow::ClientWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientWindow)
    , m_userId(-1)
    , m_currentCartId(-1)
    , m_isCheckoutInProgress(false)
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
                for (int i = 0; i < rows.size(); ++i) {
                    QStringList cols = rows[i].split(";");
                    QList<QStandardItem*> items;
                    for (const QString &c : cols) {
                        items.append(new QStandardItem(c));
                    }
                    model->appendRow(items);
                }
            }
            ui->tableCatalog->setModel(model);

            ui->tableCatalog->horizontalHeader()->setStretchLastSection(true);
        }
        else if (command == "UPDATE_PROFILE_OK") {
            QMessageBox::information(this, "успех", parts.value(1));
        }
        else if (command == "UPDATE_PROFILE_FAIL") {
            QMessageBox::warning(this, "ошибка", parts.value(1));
        }

        else if (command == "CLIENT_ORDERS_DATA") {
            QStandardItemModel *model = new QStandardItemModel(this);
            model->setHorizontalHeaderLabels({"id", "дата", "сумма", "статус"});


            if (parts.size() > 1 && !parts[1].isEmpty()) {
                QStringList rows = parts[1].split("#", Qt::SkipEmptyParts);
                for (int i = 0; i < rows.size(); ++i) {
                    QStringList cols = rows[i].split(";");
                    QList<QStandardItem*> items;
                    for (const QString &c : cols) {
                        items.append(new QStandardItem(c));
                    }
                    model->appendRow(items);
                }
            }
            ui->tableHistory->setModel(model);
            ui->tableHistory->horizontalHeader()->setStretchLastSection(true);
        }

        else if (command == "CREATE_CART_OK") {
            m_currentCartId = parts.value(1).toInt();
            ui->btnSubmitOrder->setEnabled(true);
            updateCartDisplay();
        }
        else if (command == "CREATE_CART_FAIL") {
            ui->btnSubmitOrder->setEnabled(true);
            QMessageBox::warning(this, "Ошибка", "Не удалось создать новую корзину: " + parts.value(1));
        }
        else if (command == "ADD_TO_CART_OK") {
            QMessageBox::information(this, "Успех", "Товар добавлен в корзину");
            updateCartDisplay();
        }
        else if (command == "ADD_TO_CART_FAIL") {
            QMessageBox::warning(this, "Ошибка", parts.value(1));
        }
        else if (command == "CART_DATA") {
            QString itemsStr = parts.value(1);
            double total = parts.value(2).toDouble();
            QStandardItemModel *model = new QStandardItemModel(this);
            model->setHorizontalHeaderLabels({"ID", "Название", "Кол-во", "Цена за ед.", "Сумма"});
            if (!itemsStr.isEmpty()) {
                for (const QString &row : itemsStr.split("#", Qt::SkipEmptyParts)) {
                    QStringList cols = row.split(";");
                    if (cols.size() >= 4) {
                        int qty = cols[2].toInt();
                        double price = cols[3].toDouble();
                        double sum = qty * price;
                        QList<QStandardItem*> items;
                        items.append(new QStandardItem(cols[0]));
                        items.append(new QStandardItem(cols[1]));
                        items.append(new QStandardItem(cols[2]));
                        items.append(new QStandardItem(cols[3]));
                        items.append(new QStandardItem(QString::number(sum, 'f', 2)));
                        model->appendRow(items);
                    }
                }
            }
            ui->tableCart->setModel(model);
            ui->tableCart->horizontalHeader()->setStretchLastSection(true);
            this->setWindowTitle(QString("Кабинет Покупателя (Итого: %1 руб.)").arg(total));
        }
        else if (command == "CHECKOUT_OK") {
            m_isCheckoutInProgress = false;
            ui->btnSubmitOrder->setEnabled(true);
            QMessageBox::information(this, "Успех", "Заказ оформлен");
            m_currentCartId = -1;
            ui->tableCart->setModel(nullptr);

            ClientManager::getInstance()->sendRequest("CREATE_CART");
        }
        else if (command == "CHECKOUT_FAIL") {
            m_isCheckoutInProgress = false;
            ui->btnSubmitOrder->setEnabled(true);
            QMessageBox::warning(this, "Ошибка", parts.value(1));
        }
        else if (command == "GET_CART_FAIL") {
            if (parts.value(1).contains("не является вашей корзиной")) {
                m_currentCartId = -1;
                ui->tableCart->setModel(nullptr);
            } else {
                QMessageBox::warning(this, "Ошибка", parts.value(1));
            }
        }
        else if (command == "ACCESS_DENIED")
        {
            QMessageBox::warning(
                this,
                "Доступ запрещён",
                "Недостаточно прав для выполнения операции"
                );
        }
    });



    connect(ui->btnCatalog, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageCatalog);
        ClientManager::getInstance()->sendRequest("GET_PRODUCTS");
    });

    connect(ui->btnNewOrder, &QPushButton::clicked, this, [this]() {
        if (m_currentCartId == -1) {
            ClientManager::getInstance()->sendRequest("CREATE_CART");
        } else {
            ui->stackedWidget->setCurrentWidget(ui->pageNewOrder);
            ui->btnSubmitOrder->setEnabled(true);
            updateCartDisplay();
        }
    });

    connect(ui->btnHistory, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageHistory);
        ClientManager::getInstance()->sendRequest("GET_CLIENT_ORDERS");
    });

    connect(ui->btnProfile, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageProfile);
    });

    connect(ui->btnSaveProfile, &QPushButton::clicked, this, [this]() {
        QString name = ui->lineFirstName->text();
        QString surname = ui->lineLastName->text();
        QString phone = ui->linePhone->text();
        QString email = ui->lineEmail->text();
        QString password = ui->linePassword->text();

        QString req = QString("UPDATE_PROFILE|%1|%2|%3|%4|%5|%6")
                          .arg(m_userId)
                          .arg(surname).arg(name).arg(email).arg(phone).arg(password);
        ClientManager::getInstance()->sendRequest(req);
    });

    connect(ui->btnSubmitOrder, &QPushButton::clicked, this, [this]() {
        if (m_currentCartId == -1) {
            QMessageBox::warning(this, "Ошибка", "Нет активной корзины");
            return;
        }
        if (m_isCheckoutInProgress) return;
        m_isCheckoutInProgress = true;
        ui->btnSubmitOrder->setEnabled(false);
        ClientManager::getInstance()->sendRequest(QString("CHECKOUT|%1").arg(m_currentCartId));
    });

    connect(ui->btnAddToBasket, &QPushButton::clicked, this, [this]() {
        if (m_currentCartId == -1) {
            QMessageBox::warning(this, "Ошибка", "Сначала создайте корзину (нажмите «Оформить заказ» для создания)");
            return;
        }
        int productId = ui->lineProductId->text().toInt();
        int quantity = ui->spinQuantity->value();
        if (productId <= 0 || quantity <= 0) {
            QMessageBox::warning(this, "Ошибка", "Введите корректный ID товара и количество");
            return;
        }
        ClientManager::getInstance()->sendRequest(
            QString("ADD_TO_CART|%1|%2|%3").arg(m_currentCartId).arg(productId).arg(quantity)
            );
    });
}

ClientWindow::~ClientWindow()
{
    delete ui;
}


void ClientWindow::updateCartDisplay() {
    qDebug() << "updateCartDisplay called with m_currentCartId =" << m_currentCartId;
    if (m_currentCartId != -1) {
        ClientManager::getInstance()->sendRequest(QString("GET_CART|%1").arg(m_currentCartId));
    }
}
