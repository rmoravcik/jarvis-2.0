#include <QDebug>

#include "../firmware/strings.h"

#include "bluetooth.h"

Bluetooth::Bluetooth(QObject *parent) : QObject(parent)
{
    m_device = new QBluetoothLocalDevice();
    m_address = new QBluetoothAddress("98:D3:31:70:14:79");

    m_socket = new QBluetoothSocket(QBluetoothSocket::RfcommSocket);

    m_request = REQUEST_NO_REQUEST;
    m_intensityDevice = DeviceEyes;
    m_repulsor = RepulsorLeft;
    m_revision.clear();
    m_build.clear();

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
        sendData(BLUETOOTH_CMD_HELMET + QString(" ") + BLUETOOTH_PARAM_OPEN + QString("\r\n"));
    } else
    {
        sendData(BLUETOOTH_CMD_HELMET + QString(" ") + BLUETOOTH_PARAM_CLOSE + QString("\r\n"));
    }
    m_request = REQUEST_HELMET;
}

void Bluetooth::getHelmet(void)
{
    sendData(BLUETOOTH_CMD_HELMET + QString("\r\n"));
    m_request = REQUEST_HELMET;
}

void Bluetooth::setEyes(PowerState state)
{
    if (state == PowerOn)
    {
        sendData(BLUETOOTH_CMD_EYES + QString(" ") + BLUETOOTH_PARAM_ON + QString("\r\n"));
    } else
    {
        sendData(BLUETOOTH_CMD_EYES + QString(" ") + BLUETOOTH_PARAM_OFF + QString("\r\n"));
    }
    m_request = REQUEST_EYES;
}

void Bluetooth::getEyes(void)
{
    sendData(BLUETOOTH_CMD_EYES + QString("\r\n"));
    m_request = REQUEST_EYES;
}

void Bluetooth::setRepulsors(PowerState state)
{
    if (state == PowerOn)
    {
        sendData(BLUETOOTH_CMD_REPULSORS + QString(" ") + BLUETOOTH_PARAM_ON + QString("\r\n"));
    } else
    {
        sendData(BLUETOOTH_CMD_REPULSORS + QString(" ") + BLUETOOTH_PARAM_OFF + QString("\r\n"));
    }
    m_request = REQUEST_REPULSORS;
}

void Bluetooth::getRepulsors(void)
{
    sendData(BLUETOOTH_CMD_REPULSORS + QString("\r\n"));
    m_request = REQUEST_REPULSORS;
}

void Bluetooth::repulsorsBlast(Repulsor repulsor)
{
    if (repulsor == RepulsorLeft)
    {
        sendData(BLUETOOTH_CMD_REPULSOR + QString(" ") + BLUETOOTH_PARAM_LEFT + QString("\r\n"));
    } else
    {
        sendData(BLUETOOTH_CMD_REPULSOR + QString(" ") + BLUETOOTH_PARAM_RIGHT + QString("\r\n"));
    }
    m_request = REQUEST_REPULSOR;
    m_repulsor = repulsor;
}


void Bluetooth::setUnibeam(PowerState state)
{
    if (state == PowerOn)
    {
        sendData(BLUETOOTH_CMD_UNIBEAM + QString(" ") + BLUETOOTH_PARAM_ON + QString("\r\n"));
    } else
    {
        sendData(BLUETOOTH_CMD_UNIBEAM + QString(" ") + BLUETOOTH_PARAM_OFF + QString("\r\n"));
    }
    m_request = REQUEST_UNIBEAM;
}

void Bluetooth::getUnibeam(void)
{
    sendData(BLUETOOTH_CMD_UNIBEAM + QString("\r\n"));
    m_request = REQUEST_UNIBEAM;
}

void Bluetooth::setIntensity(Device device, PowerIntensity intensity)
{
    if (device == DeviceEyes) {
        sendData(BLUETOOTH_CMD_INTENSITY + QString(" ") + BLUETOOTH_CMD_EYES + QString(" "));
    } else if (device == DeviceRepulsors) {
        sendData(BLUETOOTH_CMD_INTENSITY + QString(" ") + BLUETOOTH_CMD_REPULSORS + QString(" "));
    } else if (device == DeviceUnibeam) {
        sendData(BLUETOOTH_CMD_INTENSITY + QString(" ") + BLUETOOTH_CMD_UNIBEAM + QString(" "));
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
    m_request = REQUEST_INTENSITY;
    m_intensityDevice = device;
}

void Bluetooth::getIntensity(Device device)
{
    if (device == DeviceEyes) {
        sendData(BLUETOOTH_CMD_INTENSITY + QString(" ") + BLUETOOTH_CMD_EYES + QString("\r\n"));
    } else if (device == DeviceRepulsors) {
        sendData(BLUETOOTH_CMD_INTENSITY + QString(" ") + BLUETOOTH_CMD_REPULSORS + QString("\r\n"));
    } else if (device == DeviceUnibeam) {
        sendData(BLUETOOTH_CMD_INTENSITY + QString(" ") + BLUETOOTH_CMD_UNIBEAM + QString("\r\n"));
    }
    m_request = REQUEST_INTENSITY;
    m_intensityDevice = device;
}

void Bluetooth::setVolume(VolumeLevel level)
{
    switch (level) {
        case Level0:
            sendData(BLUETOOTH_CMD_VOLUME + QString(" 0\r\n"));
            break;
        case Level1:
            sendData(BLUETOOTH_CMD_VOLUME + QString(" 1\r\n"));
            break;
        case Level2:
            sendData(BLUETOOTH_CMD_VOLUME + QString(" 2\r\n"));
            break;
        case Level3:
            sendData(BLUETOOTH_CMD_VOLUME + QString(" 3\r\n"));
            break;
        case Level4:
            sendData(BLUETOOTH_CMD_VOLUME + QString(" 4\r\n"));
            break;
        case Level5:
            sendData(BLUETOOTH_CMD_VOLUME + QString(" 5\r\n"));
            break;
        case Level6:
            sendData(BLUETOOTH_CMD_VOLUME + QString(" 6\r\n"));
            break;
        case Level7:
            sendData(BLUETOOTH_CMD_VOLUME + QString(" 7\r\n"));
            break;
    }
    m_request = REQUEST_VOLUME;
}

void Bluetooth::getVolume(void)
{
    sendData(BLUETOOTH_CMD_VOLUME + QString("\r\n"));
    m_request = REQUEST_VOLUME;
}

void Bluetooth::playQuote(void)
{
    sendData(BLUETOOTH_CMD_FORTUNE + QString("\r\n"));
    m_request = REQUEST_FORTUNE;
}

void Bluetooth::getBattery(void)
{
    sendData(BLUETOOTH_CMD_BATTERY + QString("\r\n"));
    m_request = REQUEST_BATTERY;
}

void Bluetooth::getVersion(void)
{
    m_revision.clear();
    m_build.clear();

    sendData(BLUETOOTH_CMD_VERSION + QString("\r\n"));
    m_request = REQUEST_VERSION;
}

void Bluetooth::reboot(void)
{
    sendData(BLUETOOTH_CMD_REBOOT + QString("\r\n"));
    m_request = REQUEST_REBOOT;
}

void Bluetooth::onConnected(void)
{
    qDebug() << "Connected...";
    sendData("\r\n");
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
    if (!m_socket)
        return;
    while (m_socket->canReadLine()) {
        QByteArray data = m_socket->readLine();
        QString line = QString::fromUtf8(data.constData(), data.length()).trimmed();
        if (line.length() > 0) {
            qDebug() << "Received:" << line << "request:" << m_request;

            if (!line.contains(BLUETOOTH_PROMPT)) {
                switch (m_request) {
                    case REQUEST_BATTERY:
                        if (line.contains(BLUETOOTH_CMD_BATTERY)) {
                            line.remove(BLUETOOTH_CMD_BATTERY + QString(": "));
                            line.remove(QChar('%'));
                            unsigned int capacity = line.toInt();
                            emit battery(capacity);
                        }
                        m_request = REQUEST_NO_REQUEST;
                        break;

                    case REQUEST_EYES:
                        if (line.contains(BLUETOOTH_CMD_EYES)) {
                            PowerState state;
                            line.remove(BLUETOOTH_CMD_EYES + QString(": "));
                            if (line.contains(BLUETOOTH_PARAM_ON)) {
                                state = PowerOn;
                            } else {
                                state = PowerOff;
                            }
                            emit eyes(state);
                        } else {
                            getEyes();
                        }
                        m_request = REQUEST_NO_REQUEST;
                        break;

                    case REQUEST_FORTUNE:
                        if (line.contains("OK")) {
                            emit quoteFinished();
                        }
                        m_request = REQUEST_NO_REQUEST;
                        break;

                    case REQUEST_HELMET:
                        if (line.contains(BLUETOOTH_CMD_HELMET)) {
                            HelmetState state;
                            line.remove(BLUETOOTH_CMD_HELMET + QString(": "));
                            if (line.contains(BLUETOOTH_PARAM_CLOSE)) {
                                state = HelmetClose;
                            } else {
                                state = HelmetOpen;
                            }
                            emit helmet(state);
                        } else {
                            getHelmet();
                        }
                        m_request = REQUEST_NO_REQUEST;
                        break;

                    case REQUEST_INTENSITY:
                        if (line.contains(BLUETOOTH_CMD_INTENSITY)) {
                            line.remove(BLUETOOTH_CMD_INTENSITY + QString(": "));
                            PowerIntensity value = (PowerIntensity) line.toInt();
                            emit intensity(m_intensityDevice, value);
                        } else {
                            getIntensity(m_intensityDevice);
                        }
                        m_request = REQUEST_NO_REQUEST;
                        break;

                    case REQUEST_REBOOT:
                        if (line.contains("OK")) {
                            emit rebootStarted();
                        }
                        m_request = REQUEST_NO_REQUEST;
                        break;

                    case REQUEST_REPULSOR:
                        if (line.contains("OK")) {
                            emit repulsorBlastGenerated(m_repulsor);
                        }
                        m_request = REQUEST_NO_REQUEST;
                        break;

                    case REQUEST_REPULSORS:
                        if (line.contains(BLUETOOTH_CMD_REPULSORS)) {
                            PowerState state;
                            line.remove(BLUETOOTH_CMD_REPULSORS + QString(": "));
                            if (line.contains(BLUETOOTH_PARAM_ON)) {
                                state = PowerOn;
                            } else {
                                state = PowerOff;
                            }
                            emit repulsors(state);
                        } else {
                            getRepulsors();
                        }
                        m_request = REQUEST_NO_REQUEST;
                        break;

                    case REQUEST_UNIBEAM:
                        if (line.contains(BLUETOOTH_CMD_UNIBEAM)) {
                            PowerState state;
                            line.remove(BLUETOOTH_CMD_UNIBEAM + QString(": "));
                            if (line.contains(BLUETOOTH_PARAM_ON)) {
                                state = PowerOn;
                            } else {
                                state = PowerOff;
                            }
                            emit unibeam(state);
                        } else {
                            getUnibeam();
                        }
                        m_request = REQUEST_NO_REQUEST;
                        break;

                    case REQUEST_VERSION:
                        if (m_revision.isEmpty()) {
                            m_revision = line;
                        } else if (m_build.isEmpty()) {
                            m_build = line;
                            emit version(m_revision, m_build);
                            m_request = REQUEST_NO_REQUEST;
                        }
                        break;

                    case REQUEST_VOLUME:
                        if (line.contains(BLUETOOTH_CMD_VOLUME)) {
                            line.remove(BLUETOOTH_CMD_VOLUME + QString(": "));
                            VolumeLevel level = (VolumeLevel) line.toInt();
                            emit volume(level);
                        }
                        m_request = REQUEST_NO_REQUEST;
                        break;

                    default:
                        break;
                }
            }
        }
    }
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
        qDebug() << "Sending:" << data.trimmed();
    }
}
