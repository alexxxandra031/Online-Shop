#ifndef AUTHWINDOW_H
#define AUTHWINDOW_H

#include <QMainWindow>
#include "clientmanager.h"
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class AuthWindow; }
QT_END_NAMESPACE

class AuthWindow : public QMainWindow
{
    Q_OBJECT

public:
    AuthWindow(QWidget *parent = nullptr);
    ~AuthWindow();
    void setUserData(int userId) { m_userId = userId; }

private:
    Ui::AuthWindow *ui;
    int m_userId;
};

#endif // AUTHWINDOW_H
