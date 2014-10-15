#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "bluetooth.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qmlRegisterType<Bluetooth>("Bluetooth", 1, 0, "Bluetooth");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
