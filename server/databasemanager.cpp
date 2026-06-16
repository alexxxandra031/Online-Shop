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
    db.setDatabaseName("onlineshop_db");     // имя бд
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

bool DatabaseManager::loginUser(const QString& email, const QString& password, QStringList& outRoles, int& outId) {
    QSqlQuery query;

    query.prepare("SELECT id_user, password_hash FROM users WHERE email = :email");
    query.bindValue(":email", email);

    if (!query.exec() || !query.next()) return false;

    int id_user = query.value(0).toInt();
    QString db_hash = query.value(1).toString();


    if (db_hash != password) return false;
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

bool DatabaseManager::registerClient(const QString& surname, const QString& name, const QString& email, const QString& phone, const QString& password) {
    QSqlDatabase db = QSqlDatabase::database();
    db.transaction();


    QSqlQuery qUser;
    qUser.prepare("INSERT INTO users (email, password_hash) VALUES (:email, :pass) RETURNING id_user");
    qUser.bindValue(":email", email);
    qUser.bindValue(":pass", password);

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
    qClient.prepare("INSERT INTO clients (surname, name, phone, id_user) VALUES (:surname, :name, :phone, :id_user)");
    qClient.bindValue(":surname", surname);
    qClient.bindValue(":name", name);
    qClient.bindValue(":phone", phone);
    qClient.bindValue(":id_user", id_user);

    if (!qClient.exec()) {
        db.rollback();
        return false;
    }

    db.commit();
    return true;
}

bool DatabaseManager::updateClientProfile(int id_user, const QString& surname, const QString& name, const QString& email, const QString& phone, const QString& password) {
    QSqlDatabase db = QSqlDatabase::database();
    db.transaction();


    QSqlQuery qUser;
    qUser.prepare("UPDATE users SET email = :email, password_hash = :pass WHERE id_user = :id_user");
    qUser.bindValue(":email", email);
    qUser.bindValue(":pass", password);
    qUser.bindValue(":id_user", id_user);

    if (!qUser.exec()) {
        db.rollback();
        return false;
    }


    QSqlQuery qClient;
    qClient.prepare("UPDATE clients SET surname = :surname, name = :name, phone = :phone WHERE id_user = :id_user");
    qClient.bindValue(":surname", surname);
    qClient.bindValue(":name", name);
    qClient.bindValue(":phone", phone);
    qClient.bindValue(":id_user", id_user);

    if (!qClient.exec()) {
        db.rollback();
        return false;
    }

    db.commit();
    return true;
}

QStringList DatabaseManager::getAllClients() {
    QStringList list;
    QSqlQuery query("SELECT c.id_client, c.surname, c.name, u.email, c.phone, c.status "
                    "FROM clients c "
                    "JOIN users u ON c.id_user = u.id_user");
    while (query.next()) {
        QString row = QString("%1;%2;%3;%4;%5;%6")
        .arg(query.value(0).toInt())       // id_client
            .arg(query.value(1).toString())    // surname
            .arg(query.value(2).toString())    // name
            .arg(query.value(3).toString())    // email из таблицы users
            .arg(query.value(4).toString())    // phone
            .arg(query.value(5).toString());   // status
        list.append(row);
    }
    return list;
}

bool DatabaseManager::deleteClient(int id_client) {
    QSqlQuery query;
    // удаление из users, а  ON DELETE CASCADE удалит профиль из clients и user_roles
    query.prepare("DELETE FROM users WHERE id_user = (SELECT id_user FROM clients WHERE id_client = :id_client)");
    query.bindValue(":id_client", id_client);
    return query.exec();
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

bool DatabaseManager::deleteProduct(int id_product) {
    QSqlQuery query;
    query.prepare("DELETE FROM products WHERE id_product = :id");
    query.bindValue(":id", id_product);
    return query.exec();
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

bool DatabaseManager::deleteCategory(int id_category) {
    QSqlQuery query;
    query.prepare("DELETE FROM categories WHERE id_category = :id");
    query.bindValue(":id", id_category);
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
    qOrder.prepare("INSERT INTO orders (id_client) VALUES (:id_client) RETURNING id_order");
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

bool DatabaseManager::deleteDiscount(int id_discount) {
    QSqlQuery query;
    query.prepare("DELETE FROM discounts WHERE id_discount = :id");
    query.bindValue(":id", id_discount);
    return query.exec();
}

bool DatabaseManager::addManager(const QString& email, const QString& password) {
    QSqlDatabase db = QSqlDatabase::database();
    db.transaction();


    QSqlQuery qUser;
    qUser.prepare("INSERT INTO users (email, password_hash) VALUES (:email, :pass) RETURNING id_user");
    qUser.bindValue(":email", email);
    qUser.bindValue(":pass", password);

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


