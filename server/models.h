#ifndef MODELS_H
#define MODELS_H

#include <QString>

// категория товара
struct Category {
    int id;
    QString name;
};

// товар
struct Product {
    int id;
    QString name;
    double price;
    QString unit;
    int stockQuantity;
    int categoryId;
};

// пользователь системы (для авторизации и ролей)
struct User {
    int id;
    QString login;
    QString password;
    QString role; // "admin", "manager", "client"
};

#endif // MODELS_H
