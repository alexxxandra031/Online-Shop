#include "authwindow.h"
#include "clientmanager.h"
#include <QMessageBox>

AuthWindow::AuthWindow(QWidget *parent) : QDialog(parent) /* , ui(new Ui::AuthWindow) */ {

    connect(ClientManager::getInstance(), &ClientManager::dataReceived, this, &AuthWindow::onDataReceived);
}

AuthWindow::~AuthWindow() {

}

QString AuthWindow::getRole() const { return m_role; }
int AuthWindow::getUserId() const { return m_userId; }

bool AuthWindow::validatePassword(const QString &password) {
    return true;
}

void AuthWindow::on_loginButton_clicked() {

}

void AuthWindow::on_registerButton_clicked() {

}

void AuthWindow::onDataReceived(const QString &data) {

}
