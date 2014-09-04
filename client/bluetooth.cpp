#include <QDebug>

#include "bluetooth.h"

Bluetooth::Bluetooth(QObject *parent) : QObject(parent)
{
    m_device = new QBluetoothLocalDevice();
    m_address = new QBluetoothAddress("98:D3:31:70:14:79");

    m_socket = new QBluetoothSocket(QBluetoothSocket::RfcommSocket);

    connect(m_socket, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(m_socket, SIGNAL(error(QBluetoothSocket::SocketError)),
            this, SLOT(onError(QBluetoothSocket::SocketError)));

    if (m_device->isValid()) {
        m_device->powerOn();
    }
}

Bluetooth::~Bluetooth()
{
    if (m_device->isValid()) {
        m_device->setHostMode(QBluetoothLocalDevice::HostPoweredOff);
    }

    delete m_socket;
    delete m_device;
    delete m_address;
}

void Bluetooth::start(void)
{
    m_socket->connectToService(*m_address, 1);
}

void Bluetooth::stop(void)
{
    m_socket->disconnectFromService();
}

void Bluetooth::onConnected(void)
{
    qDebug() << "Connected...";
    emit jarvisConnected();
}

void Bluetooth::onDisconnected(void)
{
    qDebug() << "Disconnected...";
    emit jarvisDisconnected();
}

void Bluetooth::onReadyRead(void)
{
    qDebug() << "Ready read...";
}

void Bluetooth::onError(QBluetoothSocket::SocketError error)
{
    qDebug() << "Error: " << error << m_socket->errorString();
}
