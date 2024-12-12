#ifndef SOCKETMANAGER_H
#define SOCKETMANAGER_H


#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QUdpSocket>
#include <functional>

class SocketManager : public QObject
{
    Q_OBJECT

public:
    enum class Protocol { TCP, UDP };

    explicit SocketManager(QObject *parent = nullptr);
    ~SocketManager();

    // Set the callback for incoming messages
    void setMessageCallback(const std::function<void(const QByteArray &)> &callback);

    // Initialize the socket
    bool initialize(Protocol protocol, const QString &host, quint16 port, bool isServer = false);

    // Send a message
    qint64 sendMessage(const QByteArray &message);

private slots:
    void onReadyRead();

private:
    Protocol m_protocol;
    QTcpSocket *m_tcpSocket = nullptr;
    QTcpServer *m_tcpServer = nullptr;
    QUdpSocket *m_udpSocket = nullptr;
    std::function<void(const QByteArray &)> m_messageCallback;

    void handleIncomingConnection();
};

#endif // SOCKETMANAGER_H
