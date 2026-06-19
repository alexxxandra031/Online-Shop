#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void showRoleSelection(const QStringList &roles);
    void setRoles(const QStringList &roles);
    void setUserData(int id, const QStringList& roles) {
        m_userId = id;
        m_roles = roles;
    }
    int getUserId() const { return m_userId; }

private:
    Ui::MainWindow *ui;

    int m_userId;
    QStringList m_roles;


};

#endif // MAINWINDOW_H

