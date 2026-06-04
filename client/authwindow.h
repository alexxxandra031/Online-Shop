#ifndef AUTHWINDOW_H
#define AUTHWINDOW_H

#include <QDialog>
#include <QRegularExpression>

namespace Ui { class AuthWindow; }

class AuthWindow : public QDialog {
    Q_OBJECT

public:
    explicit AuthWindow(QWidget *parent = nullptr);
    ~AuthWindow();

    QString getRole() const;
    int getUserId() const;

private slots:
    void on_loginButton_clicked();
    void on_registerButton_clicked();
    void onDataReceived(const QString &data);

private:
    Ui::AuthWindow *ui;

    QString m_role;
    int m_userId = -1;

    // проверка пароля???
    bool validatePassword(const QString &password);
};

#endif // AUTHWINDOW_H
