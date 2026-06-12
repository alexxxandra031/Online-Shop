#include "clientmanager.h"
#include <QDebug>

ClientManager* ClientManager::getInstance() {
    static ClientManager instance;
    return &instance;
}

ClientManager::ClientManager(QObject *parent) : QObject(parent), m_socket(new QTcpSocket(this)) {

    connect(m_socket, &QTcpSocket::connected, this, &ClientManager::connected);
    connect(m_socket, &QTcpSocket::disconnected, this, &ClientManager::disconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &ClientManager::onReadyRead);
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        connect(m_socket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError socketError) {
            emit errorOccurred(m_socket->errorString());
        });
    #else
        connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, [this](QAbstractSocket::SocketError socketError) {
            emit errorOccurred(m_socket->errorString());
        });
    #endif
}


ClientManager::~ClientManager() {
    if (m_socket->isOpen()) {
        m_socket->close();
    }
}

void ClientManager::connectToServer(const QString &ip, quint16 port) {
    m_socket->connectToHost(ip, port);
}

void ClientManager::disconnectFromServer() {
    m_socket->disconnectFromHost();
}

void ClientManager::sendRequest(const QString &request) {
    // отправка текста на сервер
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->write(request.toUtf8());
        m_socket->flush();
    }
}

bool ClientManager::isConnected() const {
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

void ClientManager::onReadyRead() {
    // чтение ответа от сервера и передача его в окна через сигнал
    QByteArray data = m_socket->readAll();
    emit dataReceived(QString::fromUtf8(data));
}
