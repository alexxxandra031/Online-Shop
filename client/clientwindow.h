#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <QMainWindow>
#include "clientmanager.h"
#include <QStandardItemModel>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class ClientWindow; }
QT_END_NAMESPACE

class ClientWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ClientWindow(QWidget *parent = nullptr);
    ~ClientWindow();
    void setUserData(int userId) { m_userId = userId; }


private:
    Ui::ClientWindow *ui;
    int m_userId;
};

#endif // CLIENTWINDOW_H





