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

void Bluetooth::requestConnection(void)
{
    if (!m_socket) {
        return;
    }

    qDebug() << "Connecting...";
    m_socket->connectToService(*m_address, 1);
}

void Bluetooth::requestDisconnection(void)
{
    if (!m_socket) {
        return;
    }

    qDebug() << "Disconnecting...";
    m_socket->disconnectFromService();
}

bool Bluetooth::isConnected(void)
{
    if (!m_socket) {
        return false;
    }

    if (m_socket->state() == QBluetoothSocket::ConnectedState) {
                return true;
    }

    return false;
}

void Bluetooth::setHelmet(HelmetState state)
{
    if (state == HelmetOpen)
    {
        sendData("HELMET OPEN\r\n");
    } else
    {
        sendData("HELMET CLOSE\r\n");
    }
}

void Bluetooth::getHelmet(void)
{
    sendData("HELMET\r\n");
}

void Bluetooth::setEyes(PowerState state)
{
    if (state == PowerOn)
    {
        sendData("EYES ON\r\n");
    } else
    {
        sendData("EYES OFF\r\n");
    }
}

void Bluetooth::getEyes(void)
{
    sendData("EYES\r\n");
}

void Bluetooth::setRepulsors(PowerState state)
{
    if (state == PowerOn)
    {
        sendData("REPULSORS ON\r\n");
    } else
    {
        sendData("REPULSORS OFF\r\n");
    }
}

void Bluetooth::getRepulsors(void)
{
    sendData("REPULSORS\r\n");
}

void Bluetooth::repulsorsBlast(Repulsor repulsor)
{
    if (repulsor == RepulsorLeft)
    {
        sendData("REPULSOR LEFT\r\n");
    } else
    {
        sendData("REPULSOR RIGHT\r\n");
    }
}


void Bluetooth::setUnibeam(PowerState state)
{
    if (state == PowerOn)
    {
        sendData("UNIBEAM ON\r\n");
    } else
    {
        sendData("UNIBEAM OFF\r\n");
    }
}

void Bluetooth::getUnibeam(void)
{
    sendData("UNIBEAM\r\n");
}

void Bluetooth::setIntensity(Device device, PowerIntensity intensity)
{
    if (device == DeviceEyes) {
        sendData("INTENSITY EYES ");
    } else if (device == DeviceRepulsors) {
        sendData("INTENSITY REPULSORS ");
    } else if (device == DeviceUnibeam) {
        sendData("INTENSITY UNIBEAM ");
    }

    switch (intensity) {
        case Intensity10:
            sendData("0\r\n");
            break;
        case Intensity20:
            sendData("1\r\n");
            break;
        case Intensity30:
            sendData("2\r\n");
            break;
        case Intensity40:
            sendData("3\r\n");
            break;
        case Intensity50:
            sendData("4\r\n");
            break;
        case Intensity60:
            sendData("5\r\n");
            break;
        case Intensity70:
            sendData("6\r\n");
            break;
        case Intensity80:
            sendData("7\r\n");
            break;
        case Intensity90:
            sendData("8\r\n");
            break;
        case Intensity100:
            sendData("9\r\n");
            break;
    }
}

void Bluetooth::getIntensity(Device device)
{
    if (device == DeviceEyes) {
        sendData("INTENSITY EYES\r\n");
    } else if (device == DeviceRepulsors) {
        sendData("INTENSITY REPULSORS\r\n");
    } else if (device == DeviceUnibeam) {
        sendData("INTENSITY UNIBEAM\r\n");
    }
}

void Bluetooth::setVolume(VolumeLevel level)
{
    switch (level) {
        case Level0:
            sendData("VOLUME 0\r\n");
            break;
        case Level1:
            sendData("VOLUME 1\r\n");
            break;
        case Level2:
            sendData("VOLUME 2\r\n");
            break;
        case Level3:
            sendData("VOLUME 3\r\n");
            break;
        case Level4:
            sendData("VOLUME 4\r\n");
            break;
        case Level5:
            sendData("VOLUME 5\r\n");
            break;
        case Level6:
            sendData("VOLUME 6\r\n");
            break;
        case Level7:
            sendData("VOLUME 7\r\n");
            break;
    }
}

void Bluetooth::getVolume(void)
{
    sendData("VOLUME\r\n");
}

void Bluetooth::playQuote(void)
{
    sendData("FORTUNE\r\n");
}

void Bluetooth::getBattery(void)
{
    sendData("BATTERY\r\n");
}

void Bluetooth::getVersion(void)
{
    sendData("VERSION\r\n");
}

void Bluetooth::reboot(void)
{
    sendData("REBOOT\r\n");
}

void Bluetooth::onConnected(void)
{
    qDebug() << "Connected...";
    emit connected();
}

void Bluetooth::onDisconnected(void)
{
    qDebug() << "Disconnected...";
    emit disconnected();
}

void Bluetooth::onReadyRead(void)
{
    qDebug() << "Ready read...";
}

void Bluetooth::onError(QBluetoothSocket::SocketError error)
{
    qDebug() << "Error: " << error << m_socket->errorString();
}

void Bluetooth::sendData(const QString &data)
{
    if (isConnected()) {
        unsigned int len = data.length();
        m_socket->write(data.toUtf8().constData(), len);
    }
}
