#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clientwindow.h"
#include "managerwindow.h"
#include "adminwindow.h"
#include "clientmanager.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_userId(-1)
{
    ui->setupUi(this);
    // По умолчанию все кнопки скрыты, пока не установлены роли
    ui->btnRoleClient->setVisible(false);
    ui->btnRoleManager->setVisible(false);
    ui->btnRoleAdmin->setVisible(false);

    connect(ui->btnRoleClient, &QPushButton::clicked, this, [this]() {
        ClientWindow *clientWin = new ClientWindow();
        clientWin->setAttribute(Qt::WA_DeleteOnClose);
        clientWin->setUserData(m_userId);
        clientWin->show();
        this->close();
    });

    connect(ui->btnRoleManager, &QPushButton::clicked, this, [this]() {
        ManagerWindow *managerWin = new ManagerWindow();
        managerWin->setAttribute(Qt::WA_DeleteOnClose);
        managerWin->setUserData(m_userId);
        managerWin->show();
        this->close();
    });

    connect(ui->btnRoleAdmin, &QPushButton::clicked, this, [this]() {
        AdminWindow *adminWin = new AdminWindow();
        adminWin->setAttribute(Qt::WA_DeleteOnClose);
        adminWin->setUserData(m_userId);
        adminWin->show();
        this->close();
    });
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setRoles(const QStringList &roles) {
    m_roles = roles;
    ui->btnRoleClient->setVisible(roles.contains("Клиент"));
    ui->btnRoleManager->setVisible(roles.contains("Менеджер"));
    ui->btnRoleAdmin->setVisible(roles.contains("Администратор"));
}

void MainWindow::showRoleSelection(const QStringList &roles) {
    setRoles(roles);
    this->show();
}

