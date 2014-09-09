import QtQuick 1.1
import com.nokia.meego 1.0
import Bluetooth 1.0

Page {
    id: main
    width: 854
    height: 480
    tools: null
    orientationLock: PageOrientation.LockLandscape

    Timer {
        id: refresh_timer
        interval: 30000
        triggeredOnStart: true
        repeat: true

        onTriggered: {
            bluetooth.getBattery();
        }
    }

    Item {
        id: status
        property variant eyes: bluetooth.PowerOff
        property variant helmet: bluetooth.HelmetOpen
    }

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
            id: jarvis_speak
            loops: Animation.Infinite
            easing.type: Easing.OutInBounce
            from: 0
            to: 1
            duration: 500
        }

        NumberAnimation on opacity {
            id: jarvis_speak_finished
            from: 1
            to: 0
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
               if (bluetooth.isConnected()) {
                   jarvis.visible = true;
                   jarvis_speak.start();
                   terminal_log.text = terminal_log.text + "Playing quote...\n> ";
                   bluetooth.playQuote();
               }
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
               if (bluetooth.isConnected()) {
                   terminal_log.text = terminal_log.text + "Helmet...\n> ";
                   if (status.helmet == bluetooth.HelmetClose)
                       bluetooth.setHelmet(bluetooth.HelmetClose);
                   else
                       bluetooth.setHelmet(bluetooth.HelmetOpen);
               }
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
               if (bluetooth.isConnected()) {
                   terminal_log.text = terminal_log.text + "Unibeam...\n> ";
                   bluetooth.setUnibeam(bluetooth.PowerOff);
               }
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
//               if (bluetooth.isConnected()) {
                   terminal_log.text = terminal_log.text + "Version...\n> ";
                   bluetooth.getVersion();
//               }
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
            bluetooth.requestConnection();
        }
    }

    Bluetooth {
        id: bluetooth
        onConnected: {
            refresh_timer.start();
            terminal_log.text = terminal_log.text + "Connected!\n> ";
            connect_button.visible = false;
            bluetooth.getEyes();
            bluetooth.getHelmet();
        }
        onDisconnected: {
            refresh_timer.stop();
            terminal_log.text = terminal_log.text + "Disconnected!\n> ";
        }
        onBattery: {
            battery.text = "Current power level is at " + capacity + "% and holding steady.";
        }
        onEyes: {
            status.eyes = state;
            // update picture
        }
        onQuoteFinished: {
            jarvis_speak.stop();
            jarvis_speak_finished.start();
        }
        onHelmet: {
            status.helmet = state;
            // update picture
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
