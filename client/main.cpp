#include <QtGui/QApplication>
#include <QtDeclarative/QDeclarativeContext>
#include "qmlapplicationviewer.h"

#include "bluetooth.h"

Bluetooth *bt;

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    bt = new Bluetooth();

    QmlApplicationViewer viewer;
    viewer.rootContext()->setContextProperty("Bluetooth", bt);
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/jarvis/main.qml"));
    viewer.showExpanded();

    return app->exec();
}
