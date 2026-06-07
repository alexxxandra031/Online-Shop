#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clientwindow.h"
#include "managerwindow.h"
#include "adminwindow.h"



MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);


    connect(ui->btnRoleClient, &QPushButton::clicked, this, [this]() {
        ClientWindow *clientWin = new ClientWindow();
        clientWin->setAttribute(Qt::WA_DeleteOnClose);
        clientWin->show();
        this->close(); // Закрываем меню выбора ролей
    });


    connect(ui->btnRoleManager, &QPushButton::clicked, this, [this]() {
        ManagerWindow *managerWin = new ManagerWindow();
        managerWin->setAttribute(Qt::WA_DeleteOnClose);
        managerWin->show();
        this->close();
    });


    connect(ui->btnRoleAdmin, &QPushButton::clicked, this, [this]() {
        AdminWindow *adminWin = new AdminWindow();
        adminWin->setAttribute(Qt::WA_DeleteOnClose);
        adminWin->show();
        this->close();
    });
}

MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::showRoleSelection(const QStringList &roles) {
    this->show();
}




