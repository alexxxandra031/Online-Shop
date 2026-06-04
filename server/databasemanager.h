#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QList>
#include <QString>
#include "models.h"

class DatabaseManager;

class DatabaseDestroyer {
private:
    DatabaseManager* p_instance;
public:
    ~DatabaseDestroyer();
    void initialize(DatabaseManager* p);
};

class DatabaseManager {
private:
    static DatabaseManager* p_instance;
    static DatabaseDestroyer destroyer;

protected:
    DatabaseManager();

public:
    static DatabaseManager* getInstance();

    // блок работы с пользователями
    bool loginUser(const QString& email, const QString& password, QString& outRole, int& outId);
    bool registerClient(const Client& client);
    bool updateClientProfile(const Client& client);
    QList<Client> getAllClients();
    bool deleteClient(int id_client);

    // блок работы с товарами и категориями
    QList<Product> getAllProducts();
    QList<Category> getAllCategories();
    bool addProduct(const Product& product);
    bool updateProduct(const Product& product);
    bool deleteProduct(int id_product);
    bool addCategory(const Category& category);

    // блок работы со скидками
    QList<Discount> getAllDiscounts();

    // блок работы с заказами
    bool createOrder(int id_client, const QList<OrderItem>& items);
    QList<Order> getAllOrders();
    QList<Order> getClientOrders(int id_client);
    bool updateOrderDeliveryDate(int id_order, const QString& date);

    friend class DatabaseDestroyer;
};

#endif // DATABASEMANAGER_H
