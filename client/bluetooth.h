#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <QObject>

#include <QBluetoothLocalDevice>
#include <QBluetoothAddress>
#include <QBluetoothSocket>

QTM_USE_NAMESPACE

class Bluetooth : public QObject
{
    Q_OBJECT
public:
    explicit Bluetooth(QObject *parent = 0);
    ~Bluetooth();

    void start(void);
    void stop(void);

private:
    QBluetoothLocalDevice *m_device;
    QBluetoothAddress *m_address;
    QBluetoothSocket *m_socket;

signals:

public slots:
    void onConnected(void);
    void onDisconnected(void);
    void onReadyRead(void);
    void onError(QBluetoothSocket::SocketError error);
};

#endif // BLUETOOTH_H
