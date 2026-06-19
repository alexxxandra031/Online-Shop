#include "authwindow.h"
#include "ui_authwindow.h"
#include "mainwindow.h"

AuthWindow::AuthWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AuthWindow)
{
    ui->setupUi(this);

    // ответы от сервера
    connect(ClientManager::getInstance(), &ClientManager::dataReceived, this, [this](const QString &data) {
        QStringList parts = data.split("|");
        QString command = parts.value(0);

        if (command == "LOGIN_OK") {
            // parts: LOGIN_OK|role1,role2|userId
            int userId = parts.value(2).toInt();
            QString roles = parts.value(1);

            ClientManager::getInstance()->setUserData(userId, roles);

            MainWindow *mainWin = new MainWindow();
            mainWin->setAttribute(Qt::WA_DeleteOnClose);
            mainWin->showRoleSelection(roles.split(","));
            this->hide();
        }
        else if (command == "LOGIN_FAIL") {
            QMessageBox::warning(this, "ошибка", parts.value(1));
        }
        else if (command == "REGISTER_OK") {
            QMessageBox::information(this, "успех", "регистрация пройдена");
            ui->stackedWidget->setCurrentWidget(ui->pageLogin);
        }
        else if (command == "REGISTER_FAIL") {
            QMessageBox::warning(this, "ошибка", parts.value(1));
        }
    });

    connect(ClientManager::getInstance(), &ClientManager::errorOccurred, this, [this](const QString &error) {
        QMessageBox::critical(this, "ошибка сети", "не удалось подключиться к серверу:\n" + error);
    });

    connect(ui->btnGoToRegister, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageRegister);
    });


    connect(ui->btnBackToLogin, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageLogin);
    });


    connect(ui->btnLogin, &QPushButton::clicked, this, [this]() {

        QString login = ui->lineLoginEmail->text();
        QString password = ui->lineLoginPassword->text();

       if (login.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(this, "ошибка", "введите логин и пароль");
            return;
        }

        QString req = QString("LOGIN|%1|%2").arg(login).arg(password);
        ClientManager::getInstance()->sendRequest(req);
    });

    connect(ui->btnRegisterSubmit, &QPushButton::clicked, this, [this]() {
        QString login = ui->lineRegLogin->text();
        QString surname = ui->lineRegLastName->text();
        QString name = ui->lineRegFirstName->text();
        QString phone = ui->lineRegPhone->text();
        QString email = ui->lineRegEmail->text();
        QString password = ui->lineRegPassword->text();

        if (login.isEmpty() || surname.isEmpty() || name.isEmpty() || email.isEmpty() || phone.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(this, "ошибка", "заполните все обязательные поля");
            return;
        }

        QString req = QString("REGISTER|%1|%2|%3|%4|%5|%6")
                          .arg(login).arg(password).arg(surname).arg(name).arg(email).arg(phone);
        ClientManager::getInstance()->sendRequest(req);
    });
}

AuthWindow::~AuthWindow()
{
    delete ui;
}




