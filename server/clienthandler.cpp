#include "clienthandler.h"
#include "databasemanager.h"
#include <QStringList>
#include <QDebug>

ClientHandler::ClientHandler(QTcpSocket* socket, QObject *parent)
    : QObject(parent), m_socket(socket) {}

void ClientHandler::parseRequest(const QString& request) {
    QStringList parts = request.split("|", Qt::KeepEmptyParts);
    QString command = parts.value(0).trimmed();

    qDebug() << "[REQUEST]" << command;

    // авторизация
    if (command == "LOGIN") {
        if (parts.size() != 3) { sendToClient("ERROR|INVALID_FORMAT"); return; }
        handleLogin(parts[1], parts[2]);
    }
    else if (command == "REGISTER") {
        if (parts.size() != 6) { sendToClient("ERROR|INVALID_FORMAT"); return; }
        handleRegister(parts[1], parts[2], parts[3], parts[4], parts[5]);
    }
    else if (command == "UPDATE_PROFILE") {
        if (parts.size() != 7) { sendToClient("ERROR|INVALID_FORMAT"); return; }
        handleUpdateProfile(parts[1], parts[2], parts[3], parts[4], parts[5], parts[6]);
    }
    else if (command == "GET_CLIENTS") {
        handleGetClients();
    }
    else if (command == "DELETE_CLIENT") {
        if (parts.size() != 2) { sendToClient("ERROR|INVALID_FORMAT"); return; }
        handleDeleteClient(parts[1]);
    }
    // товары и категории
    else if (command == "GET_PRODUCTS") {
        handleGetProducts();
    }
    else if (command == "GET_CATEGORIES") {
        handleGetCategories();
    }
    else if (command == "ADD_PRODUCT") {
        if (parts.size() != 6) { sendToClient("ERROR|INVALID_FORMAT"); return; }
        handleAddProduct(parts[1], parts[2], parts[3], parts[4], parts[5]);
    }
    else if (command == "UPDATE_PRODUCT") {
        if (parts.size() != 7) { sendToClient("ERROR|INVALID_FORMAT"); return; }
        handleUpdateProduct(parts[1], parts[2], parts[3], parts[4], parts[5], parts[6]);
    }
    else if (command == "DELETE_PRODUCT") {
        if (parts.size() != 2) { sendToClient("ERROR|INVALID_FORMAT"); return; }
        handleDeleteProduct(parts[1]);
    }
    else if (command == "ADD_CATEGORY") {
        if (parts.size() != 2) { sendToClient("ERROR|INVALID_FORMAT"); return; }
        handleAddCategory(parts[1]);
    }
    // скидки
    else if (command == "GET_DISCOUNTS") {
        handleGetDiscounts();
    }
    // заказы
    else if (command == "CREATE_ORDER") {
        if (parts.size() != 3) { sendToClient("ERROR|INVALID_FORMAT"); return; }
        handleCreateOrder(parts[1], parts[2]);
    }
    else if (command == "GET_ALL_ORDERS") {
        handleGetAllOrders();
    }
    else if (command == "GET_CLIENT_ORDERS") {
        if (parts.size() != 2) { sendToClient("ERROR|INVALID_FORMAT"); return; }
        handleGetClientOrders(parts[1]);
    }
    else if (command == "UPDATE_DELIVERY") {
        if (parts.size() != 3) { sendToClient("ERROR|INVALID_FORMAT"); return; }
        handleUpdateDelivery(parts[1], parts[2]);
    }
    else if (command == "UPDATE_CATEGORY") {
        if (parts.size() != 3) { sendToClient("ERROR|INVALID_FORMAT"); return; }
        handleUpdateCategory(parts[1], parts[2]);
    }
    else if (command == "DELETE_CATEGORY") {
        if (parts.size() != 2) { sendToClient("ERROR|INVALID_FORMAT"); return; }
        handleDeleteCategory(parts[1]);
    }
    else if (command == "ADD_DISCOUNT") {
        if (parts.size() != 4) { sendToClient("ERROR|INVALID_FORMAT"); return; }
        handleAddDiscount(parts[1], parts[2], parts[3]);
    }
    else if (command == "UPDATE_DISCOUNT") {
        if (parts.size() != 5) { sendToClient("ERROR|INVALID_FORMAT"); return; }
        handleUpdateDiscount(parts[1], parts[2], parts[3], parts[4]);
    }
    else if (command == "DELETE_DISCOUNT") {
        if (parts.size() != 2) { sendToClient("ERROR|INVALID_FORMAT"); return; }
        handleDeleteDiscount(parts[1]);
    }
    else if (command == "ADD_MANAGER") {
        if (parts.size() != 3) { sendToClient("ERROR|INVALID_FORMAT"); return; }
        handleAddManager(parts[1], parts[2]);
    }
    else {
        sendToClient("ERROR|UNKNOWN_COMMAND");
    }
}

void ClientHandler::sendToClient(const QString& message) {
    if (m_socket && m_socket->isOpen()) {
        m_socket->write(message.toUtf8());
        m_socket->flush();
    }
}

bool ClientHandler::checkAuthorized() {
    if (m_userId == -1) {
        sendToClient("ERROR|NOT_AUTHORIZED");
        return false;
    }
    return true;
}

void ClientHandler::handleLogout() {
    m_userId = -1;
    m_role.clear();
}

// пустые заглушки методов обработки

void ClientHandler::handleLogin(const QString& email, const QString& password) {

}

void ClientHandler::handleRegister(const QString& surname, const QString& name, const QString& email, const QString& phone, const QString& password) {

}

void ClientHandler::handleUpdateProfile(const QString& id, const QString& surname, const QString& name, const QString& email, const QString& phone, const QString& password) {

}

void ClientHandler::handleGetClients() {

}

void ClientHandler::handleDeleteClient(const QString& id) {

}

void ClientHandler::handleGetProducts() {

}

void ClientHandler::handleGetCategories() {

}

void ClientHandler::handleAddProduct(const QString& name, const QString& price, const QString& unit, const QString& stock, const QString& cat_id) {

}

void ClientHandler::handleUpdateProduct(const QString& id, const QString& name, const QString& price, const QString& unit, const QString& stock, const QString& cat_id) {

}

void ClientHandler::handleDeleteProduct(const QString& id) {

}

void ClientHandler::handleAddCategory(const QString& name) {

}

void ClientHandler::handleGetDiscounts() {

}

void ClientHandler::handleCreateOrder(const QString& client_id, const QString& items_data) {

}

void ClientHandler::handleGetAllOrders() {

}

void ClientHandler::handleGetClientOrders(const QString& client_id) {

}

void ClientHandler::handleUpdateDelivery(const QString& order_id, const QString& date) {

}

void ClientHandler::handleUpdateCategory(const QString& id, const QString& name) {

}

void ClientHandler::handleDeleteCategory(const QString& id) {

}

void ClientHandler::handleAddDiscount(const QString& type, const QString& min_sum, const QString& percent) {

}

void ClientHandler::handleUpdateDiscount(const QString& id, const QString& type, const QString& min_sum, const QString& percent) {

}

void ClientHandler::handleDeleteDiscount(const QString& id) {

}


void ClientHandler::handleAddManager(const QString& email, const QString& password) {

}
