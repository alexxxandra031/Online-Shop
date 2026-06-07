#ifndef MODELS_H
#define MODELS_H

#include <QString>

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

// клиенты (и пользователи системы)
struct Client {
    int id_client;
    QString surname;
    QString name;
    QString email;
    QString phone;
    QString status; // стандартный или постоянный
    QString password_hash; // для авторизации?

};

// скидки
struct Discount {
    int id_discount;
    QString type;
    double min_order_sum;
    double percent;
};

// заказы
struct Order {
    int id_order;
    QString sale_date;
    QString delivery_date;
    double total_sum;
    int id_client;
};

// позиции заказов
struct OrderItem {
    int id_order;
    int id_product;
    int quantity;
};

// скидки заказы
struct OrderDiscount {
    int id_discount;
    int id_order;
    double discount_sum;
};

#endif // MODELS_H
