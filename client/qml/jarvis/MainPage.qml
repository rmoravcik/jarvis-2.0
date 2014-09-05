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
               terminal_log.text = terminal_log.text + "\n" + "> Playing quote...";
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
               terminal_log.text = terminal_log.text + "\n" + "> Helmet...";
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
               terminal_log.text = terminal_log.text + "\n" + "> Unibeam...";
               Bluetooth.setUnibeam(Bluetooth.PowerOff);
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
            Bluetooth.requestConnection();
        }
    }

    Connections {
        target: Bluetooth
        onConnected: {
            terminal_log.text = terminal_log.text + "\n" + "> Connected!";
            connect_button.visible = false;
        }
        onDisconnected: {
            terminal_log.text = terminal_log.text + "\n" + "> Disconnected!";
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
            text: qsTr("> Connecting...")
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
        text: qsTr("Current power level is  at 100 and holding steady.")
        anchors.horizontalCenterOffset: 0
        anchors.horizontalCenter: parent.horizontalCenter
        font.pixelSize: 12
    }
}
