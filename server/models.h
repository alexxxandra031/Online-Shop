#ifndef MODELS_H
#define MODELS_H

#include <QString>

// роли
struct Role {
    int id_role;
    QString name;
};

// пользователи
struct User {
    int id_user;
    QString login;
    QString password_hash;
};

// профили клиентов
struct Client {
    int id_client;
    QString surname;
    QString name;
    QString email;
    QString phone;
    QString status;
    int id_user;
};

// категории
struct Category {
    int id_category;
    QString name;
};

// товары
struct Product {
    int id_product;
    QString name;
    double price;
    QString unit;
    int stock_quantity;
    int id_category;
};

// скидки
struct Discount {
    int id_discount;
    QString discount_type;
    double min_order_sum;
    double discount_percent;
};

// заказы
struct Order {
    int id_order;
    QString sale_date;
    QString delivery_date;
    double total_sum;
    int id_client;
};

// товары в корзине
struct OrderItem {
    int id_order;
    int id_product;
    int quantity;
};

// информация по скидкам в чеке
struct OrderDiscount {
    int id_discount;
    int id_order;
    double discount_sum;
};

#endif // MODELS_H
