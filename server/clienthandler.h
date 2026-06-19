#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <QObject>
#include <QTcpSocket>
#include <QString>

class ClientHandler : public QObject {
    Q_OBJECT
public:
    explicit ClientHandler(QTcpSocket* socket, QObject *parent = nullptr);
    QTcpSocket* getSocket() const { return m_socket; }

    void parseRequest(const QString& request);
    void handleLogout();

private:
    void sendToClient(const QString& message);

    // проверка прав
    bool checkAuthorized();
    bool isAdmin() const
    {
        return m_role.contains("Администратор");
    }

    bool isManager() const
    {
        return m_role.contains("Менеджер");
    }

    bool isClient() const
    {
        return m_role.contains("Клиент");
    }

    // авторизация и профиль
    void handleLogin(const QString& login, const QString& password);
    void handleRegister(const QString& login, const QString& password, const QString& surname, const QString& name, const QString& email, const QString& phone);
    void handleUpdateProfile( const QString& surname, const QString& name, const QString& email, const QString& phone, const QString& password);
    void handleGetClients();
    void handleDeleteUser(const QString& id_user_str);
    void handleGetUsers();
    // товары и категории
    void handleGetProducts();
    void handleGetCategories();
    void handleAddProduct(const QString& name, const QString& price, const QString& unit, const QString& stock, const QString& cat_id);
    void handleUpdateProduct(const QString& id, const QString& name, const QString& price, const QString& unit, const QString& stock, const QString& cat_id);
    void handleAddCategory(const QString& name);
    void handleUpdateCategory(const QString& id, const QString& name);


    // скидки
    void handleGetDiscounts();
    void handleAddDiscount(const QString& type, const QString& min_sum, const QString& percent);
    void handleUpdateDiscount(const QString& id, const QString& type, const QString& min_sum, const QString& percent);


    // заказы

    void handleGetAllOrders();
    void handleGetClientOrders();
    void handleUpdateDelivery(const QString& order_id, const QString& date);

    // доп. персонал
    void handleAddManager(const QString& email, const QString& password);

    //корзина
    void handleCreateCart();
    void handleAddToCart(const QString& order_id, const QString& product_id, const QString& quantity);
    void handleGetCart(const QString& order_id);
    void handleCheckout(const QString& order_id);
    void handleRemoveFromCart(const QString& order_id, const QString& product_id);
    void handleUpdateCartQuantity(const QString& order_id, const QString& product_id, const QString& new_quantity);
    QTcpSocket* m_socket;
    int m_userId = -1;
    QString m_role;
};

#endif // CLIENTHANDLER_H
