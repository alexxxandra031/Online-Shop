#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // настройка интерфейса под конкретную роль
    void setupRole(const QString &role, int userId);

private slots:
    void onDataReceived(const QString &data);

    // слоты для кнопок интерфейса


private:
    Ui::MainWindow *ui;

    QString m_currentRole;
    int m_currentUserId;
};

#endif // MAINWINDOW_H
