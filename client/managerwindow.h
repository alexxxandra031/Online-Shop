#ifndef MANAGERWINDOW_H
#define MANAGERWINDOW_H

#include <QMainWindow>
#include "clientmanager.h"
#include <QStandardItemModel>
#include <QMessageBox>


QT_BEGIN_NAMESPACE
namespace Ui { class ManagerWindow; }
QT_END_NAMESPACE

class ManagerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ManagerWindow(QWidget *parent = nullptr);
    ~ManagerWindow();
    void setUserData(int userId) { m_userId = userId; }


private:
    Ui::ManagerWindow *ui;
    int m_userId;
};


#endif // MANAGERWINDOW_H




