#include "tcpserver.h"
#include "clienthandler.h"
#include <QDebug>

TcpServer::TcpServer(QObject *parent) : QObject(parent) {
    m_tcpServer = new QTcpServer(this);

    connect(m_tcpServer, &QTcpServer::newConnection, this, &TcpServer::slotNewConnection);

    if(!m_tcpServer->listen(QHostAddress::Any, 33333)) {
        qDebug() << "server is not started";
    } else {
        qDebug() << "server is started on port 33333";
    }
}

TcpServer::~TcpServer() {
    m_tcpServer->close();
}

void TcpServer::slotNewConnection() {
    QTcpSocket* socket = m_tcpServer->nextPendingConnection();
    ClientHandler* client = new ClientHandler(socket, this);
    m_clients.push_back(client);

    connect(socket, &QTcpSocket::readyRead, this, &TcpServer::slotServerRead);
    connect(socket, &QTcpSocket::disconnected, this, &TcpServer::slotClientDisconnected);

    qDebug() << "новый клиент подключился:" << socket->socketDescriptor();
}

void TcpServer::slotServerRead() {
    QTcpSocket* socket = (QTcpSocket*)sender();
    QByteArray array = socket->readAll();
    QString request = QString::fromUtf8(array).trimmed();

    ClientHandler* client = nullptr;
    for (int i = 0; i < m_clients.size(); ++i) {
        if (m_clients[i]->getSocket() == socket) {
            client = m_clients[i];
            break;
        }
    }

    if(client) {
        client->parseRequest(request);
    }
}

void TcpServer::slotClientDisconnected() {
    QTcpSocket* socket = (QTcpSocket*)sender();

    for (int i = 0; i < m_clients.size(); ++i) {
        if (m_clients[i]->getSocket() == socket) {
            delete m_clients[i];
            m_clients.removeAt(i);
            break;
        }
    }
    socket->deleteLater();
    qDebug() << "клиент отключился.";
}
