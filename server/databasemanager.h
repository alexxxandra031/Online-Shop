#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QList>
#include <QString>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QCryptographicHash>
#include <QDebug>
#include <QRegularExpression>
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
    static bool isPasswordValid(const QString& password);

protected:
    DatabaseManager();

public:
    static DatabaseManager* getInstance();
    int getClientIdByUserId(int id_user);

    // блок работы с пользователями
    bool loginUser(const QString& login, const QString& password, QStringList& outRoles, int& outId);
    bool registerClient(const QString& login, const QString& password, const QString& client_surname, const QString& name, const QString& email, const QString& phone);
    bool updateClientProfile(int id_user, const QString& client_surname, const QString& name, const QString& email, const QString& phone, const QString& password);
    bool deleteUser(int userId);
    QList<Client> getAllClients();
    QStringList getAllUsersData();


    // блок работы с товарами и категориями
    QList<Product> getAllProducts();
    QList<Product> getAvailableProducts();
    QList<Category> getAllCategories();
    bool addProduct(const Product& product);
    bool updateProduct(const Product& product);
    bool addCategory(const Category& category);
    bool updateCategory(const Category& category);
    bool categoryExists(int id_category);

    // блок работы со скидками
    QList<Discount> getAllDiscounts();
    bool addDiscount(const Discount& discount);
    bool updateDiscount(const Discount& discount);

    // блок работы с заказами
    bool createOrder(int id_client, const QList<OrderItem>& items);
    QList<Order> getAllOrders();
    QList<Order> getClientOrders(int id_client);
    bool updateOrderDeliveryDate(int id_order, const QString& date);

    // создание менеджера
    bool addManager(const QString& login, const QString& password);

    // корзина
    int createCart(int id_client);
    bool addToCart(int id_order, int id_product, int quantity);
    QList<OrderItem> getCartItems(int id_order);
    bool checkout(int id_order);
    bool isOrderOwnedByClientAndCart(int id_order, int id_client);
    double getOrderTotal(int id_order);
    QString getProductName(int id_product);
    double getProductPrice(int id_product);
    void cleanupAbandonedCart(int id_client);
    int getProductStock(int id_product);
    bool removeFromCart(int id_order, int id_product);
    bool updateCartQuantity(int id_order, int id_product, int newQuantity);
    QList<OrderDiscount> getOrderDiscounts(int id_order);

    friend class DatabaseDestroyer;
};

#endif // DATABASEMANAGER_H
