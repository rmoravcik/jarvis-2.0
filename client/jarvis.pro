QT += qml quick widgets bluetooth

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    bluetooth.cpp

HEADERS += \
    bluetooth.h

RESOURCES += \
    qml.qrc

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

OTHER_FILES += \
    android/AndroidManifest.xml
