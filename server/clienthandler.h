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

    // авторизация и профиль
    void handleLogin(const QString& email, const QString& password);
    void handleRegister(const QString& surname, const QString& name, const QString& email, const QString& phone, const QString& password);
    void handleUpdateProfile(const QString& id, const QString& surname, const QString& name, const QString& email, const QString& phone, const QString& password);
    void handleGetClients();
    void handleDeleteClient(const QString& id);

    // товары и категории
    void handleGetProducts();
    void handleGetCategories();
    void handleAddProduct(const QString& name, const QString& price, const QString& unit, const QString& stock, const QString& cat_id);
    void handleUpdateProduct(const QString& id, const QString& name, const QString& price, const QString& unit, const QString& stock, const QString& cat_id);
    void handleDeleteProduct(const QString& id);
    void handleAddCategory(const QString& name);

    // скидки
    void handleGetDiscounts();

    // заказы
    void handleCreateOrder(const QString& client_id, const QString& items_data);
    void handleGetAllOrders();
    void handleGetClientOrders(const QString& client_id);
    void handleUpdateDelivery(const QString& order_id, const QString& date);

    QTcpSocket* m_socket;
    int m_userId = -1;
    QString m_role;
};

#endif // CLIENTHANDLER_H
