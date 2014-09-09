#include <QtGui/QApplication>
#include <QtDeclarative/QDeclarativeContext>
#include <QtDeclarative>

#include "qmlapplicationviewer.h"

#include "bluetooth.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    qmlRegisterType<Bluetooth>("Bluetooth", 1, 0, "Bluetooth");

    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/jarvis/main.qml"));
    viewer.showExpanded();

    return app->exec();
}
