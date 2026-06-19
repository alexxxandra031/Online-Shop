#include "authwindow.h"
#include "ui_authwindow.h"
#include "mainwindow.h"
#include "clientwindow.h"
#include "adminwindow.h"
#include "managerwindow.h"

AuthWindow::AuthWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AuthWindow)
{
    ui->setupUi(this);

    // ответы от сервера
    connect(ClientManager::getInstance(), &ClientManager::dataReceived, this, [this](const QString &rawData) {
        QString data = rawData.trimmed();

        QStringList parts = data.split("|");
        QString command = parts.value(0).trimmed();


        qDebug() << "[CLIENT] Received:" << data;

        if (command == "LOGIN_OK") {
            int userId = parts.value(2).toInt();
            QString rolesStr = parts.value(1);
            QStringList roles = rolesStr.split(",");
            ClientManager::getInstance()->setUserData(userId, rolesStr);

            if (roles.size() == 1) {
                QString role = roles.first().trimmed();
                QMainWindow *win = nullptr;

                if (role == "Клиент") {
                    win = new ClientWindow();
                } else if (role == "Менеджер") {
                    win = new ManagerWindow();
                } else if (role == "Администратор") {
                    win = new AdminWindow();
                }

                if (win) {
                    if (auto *cw = qobject_cast<ClientWindow*>(win)) {
                        cw->setUserData(userId);
                    } else if (auto *mw = qobject_cast<ManagerWindow*>(win)) {
                        mw->setUserData(userId);
                    } else if (auto *aw = qobject_cast<AdminWindow*>(win)) {
                        aw->setUserData(userId);
                    }
                    win->setAttribute(Qt::WA_DeleteOnClose);
                    win->show();
                    this->hide();
                }
            } else {
                // Несколько ролей – показываем окно выбора
                MainWindow *mainWin = new MainWindow();
                mainWin->setAttribute(Qt::WA_DeleteOnClose);
                mainWin->setUserData(userId, roles);
                mainWin->setRoles(roles);   // <-- Скрываем ненужные кнопки
                mainWin->showRoleSelection(roles);
                this->hide();
            }
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
