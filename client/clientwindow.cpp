#include "clientwindow.h"
#include "ui_clientwindow.h"

ClientWindow::ClientWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientWindow)
{
    ui->setupUi(this);

    connect(ClientManager::getInstance(), &ClientManager::dataReceived, this, [this](const QString &data) {
        QStringList parts = data.split("|");
        QString command = parts.value(0);

        if (command == "PRODUCTS_DATA") {

            QStandardItemModel *model = new QStandardItemModel(this);
            model->setHorizontalHeaderLabels({"id", "название", "цена", "остаток"});

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

            // парсим заказы: id;дата;сумма;статус#...
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
        else if (command == "CREATE_ORDER_OK") {
            QMessageBox::information(this, "успех", "заказ успешно отправлен");
        }
        else if (command == "CREATE_ORDER_FAIL") {
            QMessageBox::warning(this, "ошибка", "не удалось создать заказ");
        }
    });



    connect(ui->btnCatalog, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageCatalog);
        ClientManager::getInstance()->sendRequest("GET_PRODUCTS");
    });

    connect(ui->btnNewOrder, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageNewOrder);
    });

    connect(ui->btnHistory, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageHistory);
        ClientManager::getInstance()->sendRequest("GET_CLIENT_ORDERS|101");
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

        //ЗАГЛУШКА!!!
        QString req = QString("UPDATE_PROFILE|101|%1|%2|%3|%4|%5")
                          .arg(surname).arg(name).arg(email).arg(phone).arg(password);

        ClientManager::getInstance()->sendRequest(req);
    });

    connect(ui->btnSubmitOrder, &QPushButton::clicked, this, [this]() {
        //дополнить потом
        QString req = "CREATE_ORDER|101|1:2;3:1";

        ClientManager::getInstance()->sendRequest(req);
    });
}

ClientWindow::~ClientWindow()
{
    delete ui;
}
