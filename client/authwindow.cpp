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

            MainWindow *mainWin = new MainWindow();
            mainWin->setAttribute(Qt::WA_DeleteOnClose);
            mainWin->show();
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


    connect(ui->btnGoToRegister, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageRegister);
    });


    connect(ui->btnBackToLogin, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageLogin);
    });


    connect(ui->btnLogin, &QPushButton::clicked, this, [this]() {

        QString email = ui->lineLoginEmail->text();
        QString password = ui->lineLoginPassword->text();

        if (email.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(this, "ошибка", "введите логин и пароль");
            return;
        }

        QString req = QString("LOGIN|%1|%2").arg(email).arg(password);
        ClientManager::getInstance()->sendRequest(req);
    });

    connect(ui->btnRegisterSubmit, &QPushButton::clicked, this, [this]() {
        QString surname = ui->lineRegLastName->text();
        QString name = ui->lineRegFirstName->text();
        QString phone = ui->lineRegPhone->text();
        QString email = ui->lineRegEmail->text();
        QString password = ui->lineRegPassword->text();

        if (surname.isEmpty() || name.isEmpty() || email.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(this, "ошибка", "заполните все обязательные поля");
            return;
        }

        QString req = QString("REGISTER|%1|%2|%3|%4|%5").arg(surname).arg(name).arg(email).arg(phone).arg(password);
        ClientManager::getInstance()->sendRequest(req);

    });
}

AuthWindow::~AuthWindow()
{
    delete ui;
}




