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
    Q_ENUMS(HelmetState)
    Q_ENUMS(PowerState)
    Q_ENUMS(Repulsor)
    Q_ENUMS(Device)
    Q_ENUMS(PowerIntensity)
    Q_ENUMS(VolumeLevel)

public:
    enum HelmetState {
        HelmetOpen = 0,
        HelmetClose
    };

    enum PowerState {
        PowerOff = 0,
        PowerOn
    };

    enum Repulsor {
        RepulsorLeft = 0,
        RepulsorRight
    };

    enum Device {
        DeviceEyes = 0,
        DeviceRepulsors,
        DeviceUnibeam
    };

    enum PowerIntensity {
        Intensity10 = 0,
        Intensity20,
        Intensity30,
        Intensity40,
        Intensity50,
        Intensity60,
        Intensity70,
        Intensity80,
        Intensity90,
        Intensity100
    };

    enum VolumeLevel {
        Level0 = 0,
        Level1,
        Level2,
        Level3,
        Level4,
        Level5,
        Level6,
        Level7
    };

    explicit Bluetooth(QObject *parent = 0);
    ~Bluetooth();

    Q_INVOKABLE void requestConnection(void);
    Q_INVOKABLE void requestDisconnection(void);
    Q_INVOKABLE bool isConnected(void);

    Q_INVOKABLE void setHelmet(HelmetState state);
    Q_INVOKABLE void getHelmet(void);

    Q_INVOKABLE void setEyes(PowerState state);
    Q_INVOKABLE void getEyes(void);

    Q_INVOKABLE void setRepulsors(PowerState state);
    Q_INVOKABLE void getRepulsors(void);

    Q_INVOKABLE void repulsorsBlast(Repulsor repulsor);

    Q_INVOKABLE void setUnibeam(PowerState state);
    Q_INVOKABLE void getUnibeam(void);

    Q_INVOKABLE void setIntensity(Device device, PowerIntensity intensity);
    Q_INVOKABLE void getIntensity(Device device);

    Q_INVOKABLE void setVolume(VolumeLevel level);
    Q_INVOKABLE void getVolume(void);

    Q_INVOKABLE void playQuote(void);

    Q_INVOKABLE void getBattery(void);
    Q_INVOKABLE void getVersion(void);

    Q_INVOKABLE void reboot(void);

private:
    QBluetoothLocalDevice *m_device;
    QBluetoothAddress *m_address;
    QBluetoothSocket *m_socket;

    void sendData(const QString &data);

signals:
    void connected(void);
    void disconnected(void);

public slots:
    void onConnected(void);
    void onDisconnected(void);
    void onReadyRead(void);
    void onError(QBluetoothSocket::SocketError error);
};

#endif // BLUETOOTH_H
