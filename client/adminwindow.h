#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QMainWindow>
#include "clientmanager.h"
#include <QStandardItemModel>
#include <QMessageBox>
#include <QInputDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class AdminWindow; }
QT_END_NAMESPACE

class AdminWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdminWindow(QWidget *parent = nullptr);
    ~AdminWindow();

    void setUserData(int userId) { m_userId = userId; }

private:
    Ui::AdminWindow *ui;
    int m_userId;
};


#endif // ADMINWINDOW_H
