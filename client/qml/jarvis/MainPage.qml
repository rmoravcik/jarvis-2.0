import QtQuick 1.1
import com.nokia.meego 1.0

Page {
    width: 854
    height: 480
    tools: null
    orientationLock: PageOrientation.LockLandscape

    Image {
        id: background
        x: 0
        y: 0
        width: 854
        height: 480
        visible: true
        rotation: 0
        source: "resources/background.png"
    }

    Label {
        id: label
        color: "#ffffff"
        text: "Disconnected"
        anchors.centerIn: parent
        visible: false
    }

    Button {
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: label.bottom
            topMargin: 10
        }
        text: qsTr("Connect")
        iconSource: ""
        onClicked: {
            Bluetooth.start();
        }
    }

    Connections {
        target: Bluetooth
        onJarvisConnected: {
            label.text = "Connected";
            label.visible = true;
        }
        onJarvisDisconnected: {
            label.text = "Disconnected";
            label.visible = true;
        }
    }
}
