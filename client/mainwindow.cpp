#include "mainwindow.h"
#include "clientmanager.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) /* , ui(new Ui::MainWindow) */ {
    // ui->setupUi(this);
    connect(ClientManager::getInstance(), &ClientManager::dataReceived, this, &MainWindow::onDataReceived);
}

MainWindow::~MainWindow() {
    // delete ui;
}

void MainWindow::setupRole(const QString &role, int userId) {
    m_currentRole = role;
    m_currentUserId = userId;

    if (role == "admin") {
        // показать панель админа
    } else if (role == "manager") {
        // показать панель менеджера
    } else {

    }
}

void MainWindow::onDataReceived(const QString &data) {
    // парсинг
}
