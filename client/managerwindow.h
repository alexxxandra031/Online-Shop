#ifndef MANAGERWINDOW_H
#define MANAGERWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class ManagerWindow; }
QT_END_NAMESPACE

class ManagerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ManagerWindow(QWidget *parent = nullptr);
    ~ManagerWindow();

private:
    Ui::ManagerWindow *ui;
};


#endif // MANAGERWINDOW_H




