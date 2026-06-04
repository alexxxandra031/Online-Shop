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
};

#endif // CLIENTMANAGER_H
