#include "databasemanager.h"

DatabaseManager* DatabaseManager::p_instance = nullptr;
DatabaseDestroyer DatabaseManager::destroyer;

DatabaseDestroyer::~DatabaseDestroyer() {
    delete p_instance;
}

void DatabaseDestroyer::initialize(DatabaseManager* p) {
    p_instance = p;
}

DatabaseManager::DatabaseManager() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");


    db.setHostName("127.0.0.1");       // локальный сервер БД
    db.setPort(5432);                  // стандартный порт Postgres
    db.setDatabaseName("online_shop_db");     // имя бд
    db.setUserName("postgres");        // стандартный логин


    db.setPassword("1");

    if (!db.open()) {
        qDebug() << "ошибка подключения к БД:" << db.lastError().text();
    } else {
        qDebug() << "успешное подключение к базе данных shop_db!";
    }
}

DatabaseManager* DatabaseManager::getInstance() {
    if (!p_instance) {
        p_instance = new DatabaseManager();
        destroyer.initialize(p_instance);
    }
    return p_instance;
}



QString DatabaseManager::hashPassword(const QString& password)
{
    return QString(
        QCryptographicHash::hash(
            password.toUtf8(),
            QCryptographicHash::Sha256
            ).toHex()
        );
}


bool DatabaseManager::isPasswordValid(const QString& password)
{
    QRegularExpression regex(
        "^(?=.*[A-Z])(?=.*\\d)(?=.*[^A-Za-z0-9]).{8,}$"
        );

    return regex.match(password).hasMatch();
}

int DatabaseManager::getClientIdByUserId(int id_user)
{
    QSqlQuery query;

    query.prepare(
        "SELECT id_client "
        "FROM clients "
        "WHERE id_user = :id_user"
        );

    query.bindValue(":id_user", id_user);

    if (query.exec() && query.next())
        return query.value(0).toInt();

    return -1;
}

bool DatabaseManager::loginUser(const QString& login, const QString& password, QStringList& outRoles, int& outId) {
    QSqlQuery query;
    query.prepare("SELECT id_user, password_hash FROM users WHERE login = :login"); // email -> login
    query.bindValue(":login", login);

    if (!query.exec() || !query.next()) return false;

    int id_user = query.value(0).toInt();
    QString db_hash = query.value(1).toString();

    if (db_hash != hashPassword(password))
        return false;

    outId = id_user;

    QSqlQuery roleQuery;
    roleQuery.prepare("SELECT r.name FROM roles r "
                      "JOIN user_roles ur ON r.id_role = ur.id_role "
                      "WHERE ur.id_user = :id_user");
    roleQuery.bindValue(":id_user", id_user);

    if (roleQuery.exec()) {
        while (roleQuery.next()) {
            outRoles.append(roleQuery.value(0).toString());
        }
    }
    return !outRoles.isEmpty();
}

bool DatabaseManager::registerClient(const QString& login, const QString& password, const QString& client_surname, const QString& name, const QString& email, const QString& phone) {

    if (!isPasswordValid(password))
        return false;


    QSqlDatabase db = QSqlDatabase::database();
    db.transaction();

    QSqlQuery qUser;
    qUser.prepare("INSERT INTO users (login, password_hash) VALUES (:login, :pass) RETURNING id_user");
    qUser.bindValue(":login", login);
    qUser.bindValue(":pass", hashPassword(password));

    if (!qUser.exec() || !qUser.next()) {
        db.rollback();
        return false;
    }
    int id_user = qUser.value(0).toInt();

    QSqlQuery qRole;
    qRole.prepare("INSERT INTO user_roles (id_user, id_role) VALUES (:id_user, 1)");
    qRole.bindValue(":id_user", id_user);
    if (!qRole.exec()) {
        db.rollback();
        return false;
    }

    QSqlQuery qClient;
    qClient.prepare("INSERT INTO clients (surname, name, email, phone, id_user) VALUES (:surname, :name, :email, :phone, :id_user)");
    qClient.bindValue(":surname", client_surname);
    qClient.bindValue(":name", name);
    qClient.bindValue(":email", email);
    qClient.bindValue(":phone", phone);
    qClient.bindValue(":id_user", id_user);

    if (!qClient.exec()) {
        db.rollback();
        return false;
    }

    db.commit();
    return true;
}

bool DatabaseManager::updateClientProfile(int id_user, const QString& client_surname,
                                          const QString& name, const QString& email,
                                          const QString& phone, const QString& password) {
    QSqlDatabase db = QSqlDatabase::database();
    db.transaction();


    if (!password.isEmpty()) {
        if (!isPasswordValid(password)) {
            db.rollback();
            return false;
        }

        QSqlQuery qUser;
        qUser.prepare("UPDATE users SET password_hash = :pass WHERE id_user = :id_user");
        qUser.bindValue(":pass", hashPassword(password));
        qUser.bindValue(":id_user", id_user);
        if (!qUser.exec()) {
            db.rollback();
            return false;
        }
    }

    QSqlQuery qClient;
    qClient.prepare("UPDATE clients SET surname = :surname, name = :name, email = :email, phone = :phone WHERE id_user = :id_user");
    qClient.bindValue(":surname", client_surname);
    qClient.bindValue(":name", name);
    qClient.bindValue(":email", email);
    qClient.bindValue(":phone", phone);
    qClient.bindValue(":id_user", id_user);
    if (!qClient.exec()) {
        db.rollback();
        return false;
    }

    db.commit();
    return true;
}

QList<Client> DatabaseManager::getAllClients() {
    QList<Client> list;

    QSqlQuery query("SELECT id_client, surname, name, email, phone, status, id_user FROM clients");

    while (query.next()) {
        Client client;
        client.id_client      = query.value(0).toInt();
        client.surname = query.value(1).toString();
        client.name           = query.value(2).toString();
        client.email          = query.value(3).toString();
        client.phone          = query.value(4).toString();
        client.status         = query.value(5).toString();
        client.id_user        = query.value(6).toInt();

        list.append(client);
    }

    return list;
}


QList<Product> DatabaseManager::getAllProducts() {
    QList<Product> list;
    QSqlQuery query("SELECT id_product, name, price, unit, stock_quantity, id_category FROM products");
    while (query.next()) {
        Product p;
        p.id_product = query.value(0).toInt();
        p.name = query.value(1).toString();
        p.price = query.value(2).toDouble();
        p.unit = query.value(3).toString();
        p.stock_quantity = query.value(4).toInt();
        p.id_category = query.value(5).toInt();
        list.append(p);
    }
    return list;
}

QList<Product> DatabaseManager::getAvailableProducts()
{
    QList<Product> list;

    QSqlQuery query(
        "SELECT id_product, name, price, unit, stock_quantity, id_category "
        "FROM products "
        "WHERE stock_quantity > 0"
        );

    while (query.next()) {
        Product p;

        p.id_product = query.value(0).toInt();
        p.name = query.value(1).toString();
        p.price = query.value(2).toDouble();
        p.unit = query.value(3).toString();
        p.stock_quantity = query.value(4).toInt();
        p.id_category = query.value(5).toInt();

        list.append(p);
    }

    return list;
}

bool DatabaseManager::deleteUser(int id_user)
{
    QSqlQuery query;

    query.prepare("DELETE FROM users WHERE id_user = :id_user");
    query.bindValue(":id_user", id_user);

    if (!query.exec()) {
        qDebug() << "Delete user failed:" << query.lastError().text();
        return false;
    }

    return true;
}



QList<Category> DatabaseManager::getAllCategories() {
    QList<Category> list;
    QSqlQuery query("SELECT id_category, name FROM categories");
    while (query.next()) {
        Category c;
        c.id_category = query.value(0).toInt();
        c.name = query.value(1).toString();
        list.append(c);
    }
    return list;
}


bool DatabaseManager::addProduct(const Product& product) {
    QSqlQuery query;
    query.prepare("INSERT INTO products (name, price, unit, stock_quantity, id_category) "
                  "VALUES (:name, :price, :unit, :stock, :cat_id)");
    query.bindValue(":name", product.name);
    query.bindValue(":price", product.price);
    query.bindValue(":unit", product.unit);
    query.bindValue(":stock", product.stock_quantity);
    query.bindValue(":cat_id", product.id_category);
    return query.exec();
}

bool DatabaseManager::updateProduct(const Product& product) {
    QSqlQuery query;
    query.prepare("UPDATE products SET name = :name, price = :price, unit = :unit, "
                  "stock_quantity = :stock, id_category = :cat_id WHERE id_product = :id");
    query.bindValue(":name", product.name);
    query.bindValue(":price", product.price);
    query.bindValue(":unit", product.unit);
    query.bindValue(":stock", product.stock_quantity);
    query.bindValue(":cat_id", product.id_category);
    query.bindValue(":id", product.id_product);
    return query.exec();
}

bool DatabaseManager::categoryExists(int id_category)
{
    QSqlQuery query;

    query.prepare(
        "SELECT 1 "
        "FROM categories "
        "WHERE id_category = :id"
        );

    query.bindValue(":id", id_category);

    if (!query.exec())
        return false;

    return query.next();
}

bool DatabaseManager::addCategory(const Category& category) {
    QSqlQuery query;
    query.prepare("INSERT INTO categories (name) VALUES (:name)");
    query.bindValue(":name", category.name);
    return query.exec();
}

bool DatabaseManager::updateCategory(const Category& category) {
    QSqlQuery query;
    query.prepare("UPDATE categories SET name = :name WHERE id_category = :id");
    query.bindValue(":name", category.name);
    query.bindValue(":id", category.id_category);
    return query.exec();
}


QList<Order> DatabaseManager::getAllOrders() {
    QList<Order> list;

    QSqlQuery query("SELECT id_order, sale_date, delivery_date, total_sum, id_client FROM orders");
    while (query.next()) {
        Order o;
        o.id_order = query.value(0).toInt();
        o.sale_date = query.value(1).toString();
        o.delivery_date = query.value(2).toString();
        o.total_sum = query.value(3).toDouble();
        o.id_client = query.value(4).toInt();
        list.append(o);
    }
    return list;
}

QList<Order> DatabaseManager::getClientOrders(int id_client) {
    QList<Order> list;
    QSqlQuery query;
    query.prepare("SELECT id_order, sale_date, delivery_date, total_sum, id_client FROM orders WHERE id_client = :id");
    query.bindValue(":id", id_client);
    if (query.exec()) {
        while (query.next()) {
            Order o;
            o.id_order = query.value(0).toInt();
            o.sale_date = query.value(1).toString();
            o.delivery_date = query.value(2).toString();
            o.total_sum = query.value(3).toDouble();
            o.id_client = query.value(4).toInt();
            list.append(o);
        }
    }
    return list;
}

bool DatabaseManager::createOrder(int id_client, const QList<OrderItem>& items) {
    QSqlDatabase db = QSqlDatabase::database();
    db.transaction();


    QSqlQuery qOrder;
    qOrder.prepare(
        "INSERT INTO orders "
        "(id_client, sale_date, delivery_date) "
        "VALUES "
        "(:id_client, NULL, CURRENT_DATE + INTERVAL '5 days') "
        "RETURNING id_order"
        );
    qOrder.bindValue(":id_client", id_client);

    if (!qOrder.exec() || !qOrder.next()) {
        db.rollback();
        return false;
    }
    int id_order = qOrder.value(0).toInt();


    QSqlQuery qItem;
    qItem.prepare("INSERT INTO order_items (id_order, id_product, quantity) VALUES (:id_order, :id_product, :qty)");
    for (const OrderItem& item : items) {
        qItem.bindValue(":id_order", id_order);
        qItem.bindValue(":id_product", item.id_product);
        qItem.bindValue(":qty", item.quantity);
        if (!qItem.exec()) {
            db.rollback();
            return false;
        }
    }


    QSqlQuery qCheckout;
    qCheckout.prepare("UPDATE orders SET sale_date = CURRENT_TIMESTAMP WHERE id_order = :id_order");
    qCheckout.bindValue(":id_order", id_order);
    if (!qCheckout.exec()) {
        db.rollback();
        return false;
    }

    db.commit();
    return true;
}

bool DatabaseManager::updateOrderDeliveryDate(int id_order, const QString& date) {
    QSqlQuery query;

    query.prepare("UPDATE orders SET delivery_date = TO_DATE(:date, 'DD.MM.YYYY') WHERE id_order = :id");
    query.bindValue(":date", date);
    query.bindValue(":id", id_order);
    return query.exec();
}


QList<Discount> DatabaseManager::getAllDiscounts() {
    QList<Discount> list;
    QSqlQuery query("SELECT id_discount, discount_type, min_order_sum, discount_percent FROM discounts");
    while (query.next()) {
        Discount d;
        d.id_discount = query.value(0).toInt();
        d.discount_type = query.value(1).toString();
        d.min_order_sum = query.value(2).toDouble();
        d.discount_percent = query.value(3).toDouble();
        list.append(d);
    }
    return list;
}

bool DatabaseManager::addDiscount(const Discount& discount) {
    QSqlQuery query;
    query.prepare("INSERT INTO discounts (discount_type, min_order_sum, discount_percent) VALUES (:type, :min_sum, :percent)");
    query.bindValue(":type", discount.discount_type);

    query.bindValue(":min_sum", discount.min_order_sum == 0.0 ? QVariant(QVariant::Double) : discount.min_order_sum);
    query.bindValue(":percent", discount.discount_percent);
    return query.exec();
}

bool DatabaseManager::updateDiscount(const Discount& discount) {
    QSqlQuery query;
    query.prepare("UPDATE discounts SET discount_type = :type, min_order_sum = :min_sum, discount_percent = :percent WHERE id_discount = :id");
    query.bindValue(":type", discount.discount_type);
    query.bindValue(":min_sum", discount.min_order_sum == 0.0 ? QVariant(QVariant::Double) : discount.min_order_sum);
    query.bindValue(":percent", discount.discount_percent);
    query.bindValue(":id", discount.id_discount);
    return query.exec();
}


bool DatabaseManager::addManager(const QString& login, const QString& password) {

    if (!isPasswordValid(password))
        return false;


    QSqlDatabase db = QSqlDatabase::database();
    db.transaction();

    QSqlQuery qUser;
    qUser.prepare("INSERT INTO users (login, password_hash) VALUES (:login, :pass) RETURNING id_user");
    qUser.bindValue(":login", login);
    qUser.bindValue(":pass", hashPassword(password));



    if (!qUser.exec() || !qUser.next()) {
        db.rollback();
        return false;
    }
    int id_user = qUser.value(0).toInt();


    QSqlQuery qRole;
    qRole.prepare("INSERT INTO user_roles (id_user, id_role) VALUES (:id_user, 2)");
    qRole.bindValue(":id_user", id_user);

    if (!qRole.exec()) {
        db.rollback();
        return false;
    }

    db.commit();
    return true;
}


