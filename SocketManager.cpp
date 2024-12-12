
#include "SocketManager.h"

SocketManager::SocketManager(QObject *parent)
    : QObject(parent)
{
}

SocketManager::~SocketManager()
{
    if (m_tcpSocket) delete m_tcpSocket;
    if (m_tcpServer) delete m_tcpServer;
    if (m_udpSocket) delete m_udpSocket;
}

void SocketManager::setMessageCallback(const std::function<void(const QByteArray &)> &callback)
{
    m_messageCallback = callback;
}

bool SocketManager::initialize(Protocol protocol, const QString &host, quint16 port, bool isServer)
{
    m_protocol = protocol;

    if (protocol == Protocol::TCP) {
        if (isServer) {
            m_tcpServer = new QTcpServer(this);
            connect(m_tcpServer, &QTcpServer::newConnection, this, &SocketManager::handleIncomingConnection);
            return m_tcpServer->listen(QHostAddress(host), port);
        } else {
            m_tcpSocket = new QTcpSocket(this);
            m_tcpSocket->connectToHost(host, port);
            connect(m_tcpSocket, &QTcpSocket::readyRead, this, &SocketManager::onReadyRead);
            return m_tcpSocket->waitForConnected();
        }
    } else if (protocol == Protocol::UDP) {
        m_udpSocket = new QUdpSocket(this);
        connect(m_udpSocket, &QUdpSocket::readyRead, this, &SocketManager::onReadyRead);
        return m_udpSocket->bind(QHostAddress(host), port);
    }

    return false;
}

qint64 SocketManager::sendMessage(const QByteArray &message)
{
    if (m_protocol == Protocol::TCP && m_tcpSocket) {
        return m_tcpSocket->write(message);
    } else if (m_protocol == Protocol::UDP && m_udpSocket) {
        return m_udpSocket->writeDatagram(message, QHostAddress::Broadcast, m_udpSocket->localPort());
    }

    return -1;
}

void SocketManager::onReadyRead()
{
    if (m_protocol == Protocol::TCP && m_tcpSocket) {
        QByteArray data = m_tcpSocket->readAll();
        if (m_messageCallback) {
            m_messageCallback(data);
        }
    } else if (m_protocol == Protocol::UDP && m_udpSocket) {
        while (m_udpSocket->hasPendingDatagrams()) {
            QByteArray data;
            data.resize(m_udpSocket->pendingDatagramSize());
            m_udpSocket->readDatagram(data.data(), data.size());
            if (m_messageCallback) {
                m_messageCallback(data);
            }
        }
    }
}

void SocketManager::handleIncomingConnection()
{
    if (m_tcpServer) {
        m_tcpSocket = m_tcpServer->nextPendingConnection();
        connect(m_tcpSocket, &QTcpSocket::readyRead, this, &SocketManager::onReadyRead);
    }
}
