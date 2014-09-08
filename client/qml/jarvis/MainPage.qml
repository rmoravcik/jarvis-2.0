import QtQuick 1.1
import com.nokia.meego 1.0

Page {
    id: main
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

    Image {
        id: jarvis
        x: 87
        y: 273
        width: 136
        height: 136
        visible: false
        rotation: 0
        source: "resources/jarvis.png"

        NumberAnimation on opacity {
            id: jarvisCreateAnimation
            from: 0
            to: 1
            duration: 2000
        }
    }

    Item {
        id: jarvis_button
        x: 92
        y: 277
        width: 128
        height: 128

        MouseArea {
           anchors.fill: parent
           onClicked: {
               jarvis.visible = true;
               jarvisCreateAnimation.start();
               terminal_log.text = terminal_log.text + "Playing quote...\n> ";
               Bluetooth.playQuote();
           }
       }
    }

    Item {
        id: helmet_button
        x: 367
        y: 35
        width: 120
        height: 180

        MouseArea {
           anchors.fill: parent
           onClicked: {
               terminal_log.text = terminal_log.text + "Helmet...\n> ";
               Bluetooth.setHelmet(Bluetooth.HelmetClose);
           }
       }
    }

    Item {
        id: unibeam_button
        x: 367
        y: 285
        width: 120
        height: 120

        MouseArea {
            x: 0
            y: -1
            anchors.rightMargin: 0
            anchors.bottomMargin: 1
            anchors.leftMargin: 0
            anchors.topMargin: -1
           anchors.fill: parent
           onClicked: {
               terminal_log.text = terminal_log.text + "Unibeam...\n> ";
               Bluetooth.setUnibeam(Bluetooth.PowerOff);
           }
       }
    }
    Item {
        id: suit_button
        x: 676
        y: 119
        width: 128
        height: 128
        MouseArea {
           anchors.fill: parent
           onClicked: {
               terminal_log.text = terminal_log.text + "Version...\n> ";
               Bluetooth.getVersion();
           }
       }
    }

    Button {
        id: connect_button
        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }
        text: qsTr("Connect")
        iconSource: ""
        onClicked: {
            terminal_log.text = terminal_log.text + "Connecting...\n> ";
            Bluetooth.requestConnection();
        }
    }

    Connections {
        target: Bluetooth
        onConnected: {
            Bluetooth.getBattery();
            terminal_log.text = terminal_log.text + "Connected!\n> ";
            connect_button.visible = false;
        }
        onDisconnected: {
            terminal_log.text = terminal_log.text + "Disconnected!\n> ";
        }
        onBattery: {
            battery.text = "Current power level is at " + capacity + "% and holding steady.";
        }
    }

    Flickable {
        id: flickArea
        x: 561
        y: 35
        width: 250
        height: 64
        contentWidth: terminal_log.width; contentHeight: terminal_log.height
        flickableDirection: Flickable.VerticalFlick
        clip: true

        function ensureVisible(r)
        {
            if (contentX >= r.x)
                contentX = r.x;
            else if (contentX + width <= r.x + r.width)
                contentX = r.x + r.width-width;
            if (contentY >= r.y)
                contentY = r.y;
            else if (contentY + height <= r.y + r.height)
                contentY = r.y + r.height - height;
        }

        TextEdit {
            id: terminal_log
            width: flickArea.width;
            height: flickArea.height;
            color: "#ffffff"
            text: qsTr("> ")
            activeFocusOnPress: false
            readOnly: true
            font.pixelSize: 12
            onCursorRectangleChanged: flickArea.ensureVisible(cursorRectangle)
        }
    }

    Text {
        id: battery
        x: 252
        y: 422
        color: "#ffffff"
        anchors.horizontalCenterOffset: 0
        anchors.horizontalCenter: parent.horizontalCenter
        font.pixelSize: 14
    }
}
