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
    // тут позже будет подключение к postgresql
}

DatabaseManager* DatabaseManager::getInstance() {
    if (!p_instance) {
        p_instance = new DatabaseManager();
        destroyer.initialize(p_instance);
    }
    return p_instance;
}

bool DatabaseManager::loginUser(const QString& email, const QString& password, QString& outRole, int& outId) {
    return true;
}

bool DatabaseManager::registerClient(const Client& client) {
    return true;
}

bool DatabaseManager::updateClientProfile(const Client& client) {
    return true;
}

QList<Client> DatabaseManager::getAllClients() {
    return QList<Client>();
}

bool DatabaseManager::deleteClient(int id_client) {
    return true;
}

QList<Product> DatabaseManager::getAllProducts() {
    return QList<Product>();
}

QList<Category> DatabaseManager::getAllCategories() {
    return QList<Category>();
}

bool DatabaseManager::addProduct(const Product& product) {
    return true;
}

bool DatabaseManager::updateProduct(const Product& product) {
    return true;
}

bool DatabaseManager::deleteProduct(int id_product) {
    return true;
}

bool DatabaseManager::addCategory(const Category& category) {
    return true;
}

QList<Discount> DatabaseManager::getAllDiscounts() {
    return QList<Discount>();
}

bool DatabaseManager::createOrder(int id_client, const QList<OrderItem>& items) {
    return true;
}

QList<Order> DatabaseManager::getAllOrders() {
    return QList<Order>();
}

QList<Order> DatabaseManager::getClientOrders(int id_client) {
    return QList<Order>();
}

bool DatabaseManager::updateOrderDeliveryDate(int id_order, const QString& date) {
    return true;
}
