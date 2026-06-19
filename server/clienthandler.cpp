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
    else if (command == "LOGOUT") {
        handleLogout();
    }
    else if (command == "REGISTER") {
        if (parts.size() != 7) { sendToClient("ERROR|INVALID_FORMAT"); return; }
        handleRegister(parts[1], parts[2], parts[3], parts[4], parts[5], parts[6]);
    }
    else if (command == "UPDATE_PROFILE") {
        if (parts.size() != 6) { sendToClient("ERROR|INVALID_FORMAT"); return; }
        handleUpdateProfile(parts[1], parts[2], parts[3], parts[4], parts[5]);
    }
    else if (command == "GET_CLIENTS") {
        handleGetClients();
    }
    else if (command == "DELETE_USER") {
        if (parts.size() != 2) { sendToClient("ERROR|INVALID_FORMAT"); return; }
        handleDeleteUser(parts[1]);
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
        if (parts.size() != 2) {
            sendToClient("ERROR|INVALID_FORMAT");
            return;
        }
        handleCreateOrder(parts[1]);
    }
    else if (command == "GET_ALL_ORDERS") {
        handleGetAllOrders();
    }
    else if (command == "GET_CLIENT_ORDERS") {
        handleGetClientOrders();
    }
    else if (command == "UPDATE_DELIVERY") {
        if (parts.size() != 3) { sendToClient("ERROR|INVALID_FORMAT"); return; }
        handleUpdateDelivery(parts[1], parts[2]);
    }
    else if (command == "UPDATE_CATEGORY") {
        if (parts.size() != 3) { sendToClient("ERROR|INVALID_FORMAT"); return; }
        handleUpdateCategory(parts[1], parts[2]);
    }

    else if (command == "ADD_DISCOUNT") {
        if (parts.size() != 4) { sendToClient("ERROR|INVALID_FORMAT"); return; }
        handleAddDiscount(parts[1], parts[2], parts[3]);
    }
    else if (command == "UPDATE_DISCOUNT") {
        if (parts.size() != 5) { sendToClient("ERROR|INVALID_FORMAT"); return; }
        handleUpdateDiscount(parts[1], parts[2], parts[3], parts[4]);
    }

    else if (command == "ADD_MANAGER") {
        if (parts.size() != 3) { sendToClient("ERROR|INVALID_FORMAT"); return; }
        handleAddManager(parts[1], parts[2]);
    }
    else if (command == "CREATE_CART") {
        handleCreateCart();
    }
    else if (command == "ADD_TO_CART") {
        if (parts.size() != 4) { sendToClient("ERROR|INVALID_FORMAT"); return; }
        handleAddToCart(parts[1], parts[2], parts[3]);
    }
    else if (command == "GET_CART") {
        if (parts.size() != 2) { sendToClient("ERROR|INVALID_FORMAT"); return; }
        handleGetCart(parts[1]);
    }
    else if (command == "CHECKOUT") {
        if (parts.size() != 2) { sendToClient("ERROR|INVALID_FORMAT"); return; }
        handleCheckout(parts[1]);
    }
    else {
        sendToClient("ERROR|UNKNOWN_COMMAND");
    }
}

void ClientHandler::sendToClient(const QString& message) {
    if (m_socket && m_socket->isOpen()) {
        m_socket->write((message + "\n").toUtf8());
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

    if (m_userId != -1) {
        int id_client = DatabaseManager::getInstance()->getClientIdByUserId(m_userId);
        if (id_client > 0) {
            DatabaseManager::getInstance()->cleanupAbandonedCart(id_client);
        }
    }
    m_userId = -1;
    m_role.clear();
    sendToClient("LOGOUT_OK");
}



void ClientHandler::handleLogin(const QString& login, const QString& password) {
    QStringList roles;
    int id;
    if (DatabaseManager::getInstance()->loginUser(login, password, roles, id)) {
        m_userId = id;
        m_role = roles.join(",");

        int id_client = DatabaseManager::getInstance()->getClientIdByUserId(id);
        if (id_client > 0) {
            DatabaseManager::getInstance()->cleanupAbandonedCart(id_client);
        }
        sendToClient(QString("LOGIN_OK|%1|%2").arg(m_role).arg(id));
    } else {
        sendToClient("LOGIN_FAIL|неверный логин или пароль");
    }
}

void ClientHandler::handleRegister(const QString& login, const QString& password, const QString& surname, const QString& name, const QString& email, const QString& phone) {
    if (DatabaseManager::getInstance()->registerClient(login, password, surname, name, email, phone)) {
        sendToClient("REGISTER_OK");
    } else {
        sendToClient("REGISTER_FAIL|ошибка при регистрации");
    }
}

void ClientHandler::handleUpdateProfile(
    const QString& surname,
    const QString& name,
    const QString& email,
    const QString& phone,
    const QString& password)
{
    if (!checkAuthorized())
        return;


    if (password.isEmpty()) {

        if (DatabaseManager::getInstance()->updateClientProfile(
                m_userId, surname, name, email, phone, ""))
        {
            sendToClient("UPDATE_PROFILE_OK|профиль обновлен");
        }
        else
        {
            sendToClient("UPDATE_PROFILE_FAIL|ошибка обновления");
        }
    } else {

        if (DatabaseManager::getInstance()->updateClientProfile(
                m_userId, surname, name, email, phone, password))
        {
            sendToClient("UPDATE_PROFILE_OK|профиль обновлен");
        }
        else
        {
            sendToClient("UPDATE_PROFILE_FAIL|ошибка обновления");
        }
    }
}

void ClientHandler::handleGetClients()
{
    if (!checkAuthorized())
        return;
    if (!isAdmin()) {
        sendToClient("ACCESS_DENIED");
        return;
    }

    QList<Client> clients = DatabaseManager::getInstance()->getAllClients();

    QStringList rows;
    for (const auto& c : clients) {
        QString row = QString("%1;%2;%3;%4;%5;%6")
        .arg(c.id_client)
            .arg(c.surname)
            .arg(c.name)
            .arg(c.email)
            .arg(c.phone)
            .arg(c.status);
        rows << row;
    }

    sendToClient("ALL_CLIENTS_DATA|" + rows.join("#"));
}

void ClientHandler::handleDeleteUser(const QString& id_user_str)
{
    if (!checkAuthorized())
        return;
    if (!isAdmin()) {
        sendToClient("ACCESS_DENIED");
        return;
    }


    int id_user = id_user_str.toInt();
    if (id_user == m_userId) {
        sendToClient("DELETE_CLIENT_FAIL|Нельзя удалить самого себя");
        return;
    }
    if (DatabaseManager::getInstance()->deleteUser(id_user))
        sendToClient("DELETE_CLIENT_OK");
    else
        sendToClient("DELETE_CLIENT_FAIL|Не удалось удалить клиента");
}

void ClientHandler::handleGetProducts() {
    if (!checkAuthorized())
        return;
    QList<Product> prods;

    if (isClient())
        prods = DatabaseManager::getInstance()->getAvailableProducts();
    else
        prods = DatabaseManager::getInstance()->getAllProducts();
    QStringList rows;
    for (const auto& p : prods) {

        rows << QString("%1;%2;%3;%4;%5;%6")
                    .arg(p.id_product).arg(p.name).arg(p.price)
                    .arg(p.unit).arg(p.stock_quantity).arg(p.id_category);
    }
    sendToClient("PRODUCTS_DATA|" + rows.join("#"));
}

void ClientHandler::handleGetCategories() {
    if (!checkAuthorized())
        return;
    QList<Category> categories = DatabaseManager::getInstance()->getAllCategories();
    QStringList rows;
    for (const auto& c : categories) {
        rows << QString("%1;%2").arg(c.id_category).arg(c.name);
    }
    sendToClient("CATEGORIES_DATA|" + rows.join("#"));
}

void ClientHandler::handleAddProduct(const QString& name,
                                     const QString& price,
                                     const QString& unit,
                                     const QString& stock,
                                     const QString& cat_id)
{
    if (!checkAuthorized())
        return;

    if (!isAdmin() && !isManager()) {
        sendToClient("ACCESS_DENIED");
        return;
    }

    Product p;
    p.name = name;
    p.price = price.toDouble();
    p.unit = unit;
    p.stock_quantity = stock.toInt();
    p.id_category = cat_id.toInt();

    if (!DatabaseManager::getInstance()->categoryExists(p.id_category)) {
        sendToClient("ADD_PRODUCT_FAIL|Категория не существует");
        return;
    }

    if (DatabaseManager::getInstance()->addProduct(p))
        sendToClient("ADD_PRODUCT_OK");
    else
        sendToClient("ADD_PRODUCT_FAIL|ошибка добавления");
}

void ClientHandler::handleUpdateProduct(const QString& id,
                                        const QString& name,
                                        const QString& price,
                                        const QString& unit,
                                        const QString& stock,
                                        const QString& cat_id)
{
    if (!checkAuthorized())
        return;
    if (!isAdmin() && !isManager()) {
        sendToClient("ACCESS_DENIED");
        return;
    }

    Product p;
    p.id_product = id.toInt();
    p.name = name;
    p.price = price.toDouble();
    p.unit = unit;
    p.stock_quantity = stock.toInt();
    p.id_category = cat_id.toInt();
    if (!DatabaseManager::getInstance()->categoryExists(p.id_category)) {
        sendToClient("UPDATE_PRODUCT_FAIL|Категория не существует");
        return;
    }
    if (DatabaseManager::getInstance()->updateProduct(p))
        sendToClient("UPDATE_PRODUCT_OK");
    else
        sendToClient("UPDATE_PRODUCT_FAIL|ошибка обновления");
}

void ClientHandler::handleAddCategory(const QString& name)
{
    if (!checkAuthorized())
        return;
    if (!isAdmin() && !isManager()) {
        sendToClient("ACCESS_DENIED");
        return;
    }

    Category c;
    c.name = name;

    if (DatabaseManager::getInstance()->addCategory(c))
        sendToClient("ADD_CATEGORY_OK");
    else
        sendToClient("ADD_CATEGORY_FAIL|ошибка");
}

void ClientHandler::handleUpdateCategory(const QString& id,
                                         const QString& name)
{
    if (!checkAuthorized())
        return;
    if (!isAdmin() && !isManager()) {
        sendToClient("ACCESS_DENIED");
        return;
    }

    Category c;
    c.id_category = id.toInt();
    c.name = name;

    if (DatabaseManager::getInstance()->updateCategory(c))
        sendToClient("UPDATE_CATEGORY_OK");
    else
        sendToClient("UPDATE_CATEGORY_FAIL|ошибка");
}


void ClientHandler::handleGetDiscounts()
{
    if (!checkAuthorized())
        return;
    if (!isAdmin()) {
        sendToClient("ACCESS_DENIED");
        return;
    }

    QList<Discount> discs = DatabaseManager::getInstance()->getAllDiscounts();

    QStringList rows;
    for (const auto& d : discs) {
        rows << QString("%1;%2;%3;%4")
        .arg(d.id_discount)
            .arg(d.discount_type)
            .arg(d.min_order_sum)
            .arg(d.discount_percent);
    }

    sendToClient("DISCOUNTS_DATA|" + rows.join("#"));
}

void ClientHandler::handleAddDiscount(const QString& type,
                                      const QString& min_sum,
                                      const QString& percent)
{
    if (!checkAuthorized())
        return;
    if (!isAdmin()) {
        sendToClient("ACCESS_DENIED");
        return;
    }

    Discount d;
    d.discount_type = type;
    d.min_order_sum = min_sum.toDouble();
    d.discount_percent = percent.toDouble();

    if (DatabaseManager::getInstance()->addDiscount(d))
        sendToClient("ADD_DISCOUNT_OK");
    else
        sendToClient("ADD_DISCOUNT_FAIL|ошибка");
}

void ClientHandler::handleUpdateDiscount(const QString& id,
                                         const QString& type,
                                         const QString& min_sum,
                                         const QString& percent)
{
    if (!checkAuthorized())
        return;
    if (!isAdmin()) {
        sendToClient("ACCESS_DENIED");
        return;
    }

    Discount d;
    d.id_discount = id.toInt();
    d.discount_type = type;
    d.min_order_sum = min_sum.toDouble();
    d.discount_percent = percent.toDouble();

    if (DatabaseManager::getInstance()->updateDiscount(d))
        sendToClient("UPDATE_DISCOUNT_OK");
    else
        sendToClient("UPDATE_DISCOUNT_FAIL|ошибка");
}


void ClientHandler::handleCreateOrder(const QString& items_data)
{
    if (!checkAuthorized())
        return;

    if (!isClient()) {
        sendToClient("ACCESS_DENIED");
        return;
    }
    int id_client =
        DatabaseManager::getInstance()
            ->getClientIdByUserId(m_userId);

    if (id_client <= 0) {
        sendToClient("CREATE_ORDER_FAIL|Клиент не найден");
        return;
    }

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
    if (items.isEmpty()) {
        sendToClient("CREATE_ORDER_FAIL|Пустой заказ");
        return;
    }
    if (DatabaseManager::getInstance()->createOrder(id_client, items))
        sendToClient("CREATE_ORDER_OK");
    else
        sendToClient("CREATE_ORDER_FAIL|ошибка создания заказа");
}

void ClientHandler::handleGetAllOrders()
{
    if (!checkAuthorized())
        return;
    if (!isAdmin() && !isManager()) {
        sendToClient("ACCESS_DENIED");
        return;
    }

    QList<Order> orders = DatabaseManager::getInstance()->getAllOrders();

    QStringList rows;
    for (const auto& o : orders) {
        rows << QString("%1;%2;%3;%4;%5")
        .arg(o.id_order)
            .arg(o.sale_date)
            .arg(o.delivery_date)
            .arg(o.total_sum)
            .arg(o.id_client);
    }

    sendToClient("ALL_ORDERS_DATA|" + rows.join("#"));
}




void ClientHandler::handleGetClientOrders() {
    if (!checkAuthorized()) return;
    if (!isClient()) {
        sendToClient("ACCESS_DENIED");
        return;
    }
    int id_client = DatabaseManager::getInstance()->getClientIdByUserId(m_userId);
    if (id_client <= 0) {
        sendToClient("CLIENT_ORDERS_DATA|");  // пустой ответ
        return;
    }
    QList<Order> orders = DatabaseManager::getInstance()->getClientOrders(id_client);
    QStringList rows;
    for (const auto& o : orders) {
        QString status = o.delivery_date.isEmpty() ? "в обработке" : "доставка " + o.delivery_date;
        rows << QString("%1;%2;%3;%4")
                    .arg(o.id_order)
                    .arg(o.sale_date)
                    .arg(o.total_sum)
                    .arg(status);
    }
    sendToClient("CLIENT_ORDERS_DATA|" + rows.join("#"));
}


void ClientHandler::handleUpdateDelivery(const QString& order_id,
                                         const QString& date)
{
    if (!checkAuthorized())
        return;
    if (!isAdmin() && !isManager()) {
        sendToClient("ACCESS_DENIED");
        return;
    }

    if (DatabaseManager::getInstance()->updateOrderDeliveryDate(order_id.toInt(), date))
        sendToClient("UPDATE_DELIVERY_OK");
    else
        sendToClient("UPDATE_DELIVERY_FAIL|ошибка");
}


void ClientHandler::handleAddManager(const QString& email,
                                     const QString& password)
{
    if (!checkAuthorized())
        return;
    if (!isAdmin()) {
        sendToClient("ACCESS_DENIED");
        return;
    }

    if (DatabaseManager::getInstance()->addManager(email, password))
        sendToClient("ADD_MANAGER_OK");
    else
        sendToClient("ADD_MANAGER_FAIL|ошибка добавления");
}


void ClientHandler::handleCreateCart() {
    if (!checkAuthorized()) return;
    if (!isClient()) {
        sendToClient("ACCESS_DENIED");
        return;
    }
    int id_client = DatabaseManager::getInstance()->getClientIdByUserId(m_userId);
    if (id_client <= 0) {
        sendToClient("CREATE_CART_FAIL|Клиент не найден");
        return;
    }
    int cartId = DatabaseManager::getInstance()->createCart(id_client);
    if (cartId > 0)
        sendToClient(QString("CREATE_CART_OK|%1").arg(cartId));
    else
        sendToClient("CREATE_CART_FAIL|Ошибка создания корзины");
}

void ClientHandler::handleAddToCart(const QString& order_id, const QString& product_id, const QString& quantity) {
    if (!checkAuthorized()) return;
    if (!isClient()) {
        sendToClient("ACCESS_DENIED");
        return;
    }
    int oid = order_id.toInt();
    int pid = product_id.toInt();
    int qty = quantity.toInt();
    if (qty <= 0) {
        sendToClient("ADD_TO_CART_FAIL|Количество должно быть > 0");
        return;
    }
    int id_client = DatabaseManager::getInstance()->getClientIdByUserId(m_userId);
    if (id_client <= 0) {
        sendToClient("ADD_TO_CART_FAIL|Клиент не найден");
        return;
    }
    if (!DatabaseManager::getInstance()->isOrderOwnedByClientAndCart(oid, id_client)) {
        sendToClient("ADD_TO_CART_FAIL|Заказ не является вашей корзиной");
        return;
    }
    if (DatabaseManager::getInstance()->addToCart(oid, pid, qty))
        sendToClient("ADD_TO_CART_OK");
    else
        sendToClient("ADD_TO_CART_FAIL|Ошибка добавления");
}

void ClientHandler::handleGetCart(const QString& order_id) {
    if (!checkAuthorized()) return;
    if (!isClient()) {
        sendToClient("ACCESS_DENIED");
        return;
    }
    int oid = order_id.toInt();
    int id_client = DatabaseManager::getInstance()->getClientIdByUserId(m_userId);
    if (id_client <= 0) {
        sendToClient("GET_CART_FAIL|Клиент не найден");
        return;
    }
    if (!DatabaseManager::getInstance()->isOrderOwnedByClientAndCart(oid, id_client)) {
        sendToClient("GET_CART_FAIL|Заказ не является вашей корзиной");
        return;
    }
    QList<OrderItem> items = DatabaseManager::getInstance()->getCartItems(oid);
    double total = DatabaseManager::getInstance()->getOrderTotal(oid);
    QStringList rows;
    for (const auto& item : items) {
        QString name = DatabaseManager::getInstance()->getProductName(item.id_product);
        double price = DatabaseManager::getInstance()->getProductPrice(item.id_product);
        rows << QString("%1;%2;%3;%4").arg(item.id_product).arg(name).arg(item.quantity).arg(price);
    }
    sendToClient(QString("CART_DATA|%1|%2").arg(rows.join("#")).arg(total));
}

void ClientHandler::handleCheckout(const QString& order_id) {
    if (!checkAuthorized()) return;
    if (!isClient()) {
        sendToClient("ACCESS_DENIED");
        return;
    }
    int oid = order_id.toInt();
    int id_client = DatabaseManager::getInstance()->getClientIdByUserId(m_userId);
    if (id_client <= 0) {
        sendToClient("CHECKOUT_FAIL|Клиент не найден");
        return;
    }
    if (!DatabaseManager::getInstance()->isOrderOwnedByClientAndCart(oid, id_client)) {
        sendToClient("CHECKOUT_FAIL|Заказ не является вашей корзиной");
        return;
    }
    QList<OrderItem> items = DatabaseManager::getInstance()->getCartItems(oid);
    if (items.isEmpty()) {
        sendToClient("CHECKOUT_FAIL|Корзина пуста");
        return;
    }
    for (const auto& item : items) {
        int stock = DatabaseManager::getInstance()->getProductStock(item.id_product);
        if (stock < item.quantity) {
            sendToClient(QString("CHECKOUT_FAIL|Недостаточно товара ID %1 (остаток: %2)").arg(item.id_product).arg(stock));
            return;
        }
    }
    if (DatabaseManager::getInstance()->checkout(oid)) {
        sendToClient("CHECKOUT_OK");
    } else {
        sendToClient("CHECKOUT_FAIL|Ошибка оформления");
    }
}
