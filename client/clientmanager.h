#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QString>

class ClientManager : public QObject {
    Q_OBJECT

public:
    static ClientManager* getInstance();

    void connectToServer(const QString &ip, quint16 port);
    void disconnectFromServer();
    void sendRequest(const QString &request);

    bool isConnected() const;

    void setUserData(int id, const QString& roles);
    int getUserId() const { return m_userId; }
    QString getUserRoles() const { return m_userRoles; }
    bool hasRole(const QString& role) const;

signals:
    // сигналы для обновления интерфейса
    void connected();
    void disconnected();
    void dataReceived(const QString &data);
    void errorOccurred(const QString &error);

private slots:
    void onReadyRead();

private:
    explicit ClientManager(QObject *parent = nullptr);
    ~ClientManager();

    QTcpSocket *m_socket;
    int m_userId;
    QString m_userRoles;

};

#endif // CLIENTMANAGER_H
