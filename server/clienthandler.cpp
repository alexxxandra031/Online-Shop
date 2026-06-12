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



void ClientHandler::handleLogin(const QString& email, const QString& password) {
    QString role;
    int id;

    if (DatabaseManager::getInstance()->loginUser(email, password, role, id)) {
        m_userId = id;
        m_role = role;
        sendToClient(QString("LOGIN_OK|%1|%2").arg(role).arg(id));
    } else {
        sendToClient("LOGIN_FAIL|неверный логин или пароль");
    }
}

void ClientHandler::handleRegister(const QString& surname, const QString& name, const QString& email, const QString& phone, const QString& password) {
    Client c;
    c.surname = surname; c.name = name; c.email = email; c.phone = phone; c.password_hash = password;

    if (DatabaseManager::getInstance()->registerClient(c)) {
        sendToClient("REGISTER_OK");
    } else {
        sendToClient("REGISTER_FAIL|ошибка при регистрации");
    }
}

void ClientHandler::handleUpdateProfile(const QString& id, const QString& surname, const QString& name, const QString& email, const QString& phone, const QString& password) {
    Client c;
    c.id_client = id.toInt(); c.surname = surname; c.name = name; c.email = email; c.phone = phone; c.password_hash = password;

    if (DatabaseManager::getInstance()->updateClientProfile(c)) {
        sendToClient("UPDATE_PROFILE_OK|профиль обновлен");
    } else {
        sendToClient("UPDATE_PROFILE_FAIL|ошибка обновления");
    }
}

void ClientHandler::handleGetClients() {
    QList<Client> clients = DatabaseManager::getInstance()->getAllClients();
    QStringList rows;
    for (const auto& c : clients) {
        // id;фамилия;имя;email;телефон
        rows << QString("%1;%2;%3;%4;%5").arg(c.id_client).arg(c.surname).arg(c.name).arg(c.email).arg(c.phone);
    }
    sendToClient("CLIENTS_DATA|" + rows.join("#"));
}

void ClientHandler::handleDeleteClient(const QString& id) {
    if (DatabaseManager::getInstance()->deleteClient(id.toInt())) {
        sendToClient("DELETE_CLIENT_OK");
    } else {
        sendToClient("ERROR|ошибка удаления");
    }
}

void ClientHandler::handleGetProducts() {
    QList<Product> prods = DatabaseManager::getInstance()->getAllProducts();
    QStringList rows;
    for (const auto& p : prods) {

        rows << QString("%1;%2;%3;%4;%5;%6")
                    .arg(p.id_product).arg(p.name).arg(p.price)
                    .arg(p.unit).arg(p.stock_quantity).arg(p.id_category);
    }
    sendToClient("PRODUCTS_DATA|" + rows.join("#"));
}

void ClientHandler::handleGetCategories() {
    QList<Category> categories = DatabaseManager::getInstance()->getAllCategories();
    QStringList rows;
    for (const auto& c : categories) {
        rows << QString("%1;%2").arg(c.id_category).arg(c.name);
    }
    sendToClient("CATEGORIES_DATA|" + rows.join("#"));
}

void ClientHandler::handleAddProduct(const QString& name, const QString& price, const QString& unit, const QString& stock, const QString& cat_id) {
    Product p;
    p.name = name; p.price = price.toDouble(); p.unit = unit; p.stock_quantity = stock.toInt(); p.id_category = cat_id.toInt();

    if (DatabaseManager::getInstance()->addProduct(p)) sendToClient("ADD_PRODUCT_OK");
    else sendToClient("ADD_PRODUCT_FAIL|ошибка добавления");
}

void ClientHandler::handleUpdateProduct(const QString& id, const QString& name, const QString& price, const QString& unit, const QString& stock, const QString& cat_id) {
    Product p;
    p.id_product = id.toInt(); p.name = name; p.price = price.toDouble(); p.unit = unit; p.stock_quantity = stock.toInt(); p.id_category = cat_id.toInt();

    if (DatabaseManager::getInstance()->updateProduct(p)) sendToClient("UPDATE_PRODUCT_OK");
    else sendToClient("UPDATE_PRODUCT_FAIL|ошибка обновления");
}

void ClientHandler::handleDeleteProduct(const QString& id) {
    if (DatabaseManager::getInstance()->deleteProduct(id.toInt())) sendToClient("DELETE_PRODUCT_OK");
    else sendToClient("DELETE_PRODUCT_FAIL|ошибка удаления");
}

void ClientHandler::handleAddCategory(const QString& name) {
    Category c; c.name = name;
    if (DatabaseManager::getInstance()->addCategory(c)) sendToClient("ADD_CATEGORY_OK");
    else sendToClient("ADD_CATEGORY_FAIL|ошибка");
}

void ClientHandler::handleUpdateCategory(const QString& id, const QString& name) {
    Category c; c.id_category = id.toInt(); c.name = name;
    if (DatabaseManager::getInstance()->updateCategory(c)) sendToClient("UPDATE_CATEGORY_OK");
    else sendToClient("UPDATE_CATEGORY_FAIL|ошибка");
}

void ClientHandler::handleDeleteCategory(const QString& id) {
    if (DatabaseManager::getInstance()->deleteCategory(id.toInt())) sendToClient("DELETE_CATEGORY_OK");
    else sendToClient("DELETE_CATEGORY_FAIL|ошибка");
}

void ClientHandler::handleGetDiscounts() {
    QList<Discount> discs = DatabaseManager::getInstance()->getAllDiscounts();
    QStringList rows;
    for (const auto& d : discs) {
        rows << QString("%1;%2;%3;%4").arg(d.id_discount).arg(d.type).arg(d.min_order_sum).arg(d.percent);
    }
    sendToClient("DISCOUNTS_DATA|" + rows.join("#"));
}

void ClientHandler::handleAddDiscount(const QString& type, const QString& min_sum, const QString& percent) {
    Discount d; d.type = type; d.min_order_sum = min_sum.toDouble(); d.percent = percent.toDouble();
    if (DatabaseManager::getInstance()->addDiscount(d)) sendToClient("ADD_DISCOUNT_OK");
    else sendToClient("ADD_DISCOUNT_FAIL|ошибка");
}

void ClientHandler::handleUpdateDiscount(const QString& id, const QString& type, const QString& min_sum, const QString& percent) {
    Discount d; d.id_discount = id.toInt(); d.type = type; d.min_order_sum = min_sum.toDouble(); d.percent = percent.toDouble();
    if (DatabaseManager::getInstance()->updateDiscount(d)) sendToClient("UPDATE_DISCOUNT_OK");
    else sendToClient("UPDATE_DISCOUNT_FAIL|ошибка");
}

void ClientHandler::handleDeleteDiscount(const QString& id) {
    if (DatabaseManager::getInstance()->deleteDiscount(id.toInt())) sendToClient("DELETE_DISCOUNT_OK");
    else sendToClient("DELETE_DISCOUNT_FAIL|ошибка");
}

void ClientHandler::handleCreateOrder(const QString& client_id, const QString& items_data) {
    QList<OrderItem> items;
    for (const QString& pair : items_data.split(";", Qt::SkipEmptyParts)) {
        QStringList parts = pair.split(":");
        if (parts.size() == 2) {
            OrderItem item;
            item.id_product = parts[0].toInt();
            item.quantity = parts[1].toInt();
            items.append(item);
        }
    }
    if (DatabaseManager::getInstance()->createOrder(client_id.toInt(), items)) sendToClient("CREATE_ORDER_OK");
    else sendToClient("CREATE_ORDER_FAIL|ошибка создания заказа");
}

void ClientHandler::handleGetAllOrders() {
    QList<Order> orders = DatabaseManager::getInstance()->getAllOrders();
    QStringList rows;
    for (const auto& o : orders) {
        rows << QString("%1;%2;%3;%4;%5").arg(o.id_order).arg(o.sale_date).arg(o.delivery_date).arg(o.total_sum).arg(o.id_client);
    }
    sendToClient("ALL_ORDERS_DATA|" + rows.join("#"));
}

void ClientHandler::handleGetClientOrders(const QString& client_id) {
    QList<Order> orders = DatabaseManager::getInstance()->getClientOrders(client_id.toInt());
    QStringList rows;
    for (const auto& o : orders) {
        // для клиента вместо точной даты формируем понятный статус
        QString status = o.delivery_date.isEmpty() ? "в обработке" : "доставка " + o.delivery_date;
        rows << QString("%1;%2;%3;%4").arg(o.id_order).arg(o.sale_date).arg(o.total_sum).arg(status);
    }
    sendToClient("CLIENT_ORDERS_DATA|" + rows.join("#"));
}

void ClientHandler::handleUpdateDelivery(const QString& order_id, const QString& date) {
    if (DatabaseManager::getInstance()->updateOrderDeliveryDate(order_id.toInt(), date)) sendToClient("UPDATE_DELIVERY_OK");
    else sendToClient("UPDATE_DELIVERY_FAIL|ошибка");
}


void ClientHandler::handleAddManager(const QString& email, const QString& password) {
    if (DatabaseManager::getInstance()->addManager(email, password)) sendToClient("ADD_MANAGER_OK");
    else sendToClient("ADD_MANAGER_FAIL|ошибка добавления");
}
