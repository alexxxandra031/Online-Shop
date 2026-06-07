#include "adminwindow.h"
#include "ui_adminwindow.h"

AdminWindow::AdminWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AdminWindow)
{
    ui->setupUi(this);

    connect(ui->btnCatalogCrud, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageCatalogCrud);
    });

    connect(ui->btnUsersCrud, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageUsersCrud);
    });

    connect(ui->btnDiscountsCrud, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageDiscountsCrud);
    });

    connect(ui->btnArchive, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageArchive);
    });

    connect(ui->btnProfile, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageProfile);
    });
}

AdminWindow::~AdminWindow()
{
    delete ui;
}
