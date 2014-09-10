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
        property variant eyes: Bluetooth.PowerOff
        property variant helmet: Bluetooth.HelmetOpen
        property variant intensity: [Bluetooth.Intensity50, Bluetooth.Intensity50, Bluetooth.Intensity50]
        property variant repulsors: Bluetooth.PowerOff
        property variant unibeam: Bluetooth.PowerOff
        property variant volume: Bluetooth.Level7
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

    Image {
        id: helmet
        x: 367
        y: 35
        width: 120
        height: 180
        visible: false
        rotation: 0
        source: "resources/helmet.png"
    }

    Image {
        id: unibeam
        x: 367
        y: 285
        width: 120
        height: 120
        visible: false
        rotation: 0
        source: "resources/unibeam.png"
    }

    Image {
        id: system
        x: 626
        y: 308
        width: 188
        height: 140
        visible: false
        rotation: 0
        source: "resources/system.png"

        NumberAnimation on opacity {
            id: system_connected
            from: 0
            to: 1
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
                   if (status.helmet == Bluetooth.HelmetClose) {
                       bluetooth.setHelmet(Bluetooth.HelmetOpen);
                   } else {
                       bluetooth.setHelmet(Bluetooth.HelmetClose);
                   }
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
                   if (status.unibeam == Bluetooth.PowerOn)
                       bluetooth.setUnibeam(Bluetooth.PowerOff);
                   else
                       bluetooth.setUnibeam(Bluetooth.PowerOn);
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
            system.visible = true;
            system_connected.start();
//            bluetooth.getHelmet();
//            bluetooth.getRepulsors();
//            bluetooth.getUnibeam();
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

            if (status.eyes == Bluetooth.PowerOn) {
                helmet.visible = true;
            } else {
                helmet.visible = false;
            }
        }
        onQuoteFinished: {
            jarvis_speak.stop();
            jarvis_speak_finished.start();
        }
        onHelmet: {
            status.helmet = state;

            if (status.helmet == Bluetooth.HelmetClose) {
                helmet.visible = true;
            } else {
                helmet.visible = false;
            }
        }
        onIntensity: {
            status.intensity[device] = level;
            // update picture
        }
        onRebootStarted: {
            terminal_log.text = terminal_log.text + "Rebooting...\n> ";
        }
        onRepulsorBlastGenerated: {
            // fixme
        }
        onRepulsors: {
            status.repulsors = state;
            // update picture
        }
        onUnibeam: {
            status.unibeam = state;

            if (status.unibeam == Bluetooth.PowerOn) {
                unibeam.visible = true;
            } else {
                unibeam.visible = false;
            }
        }
        onVersion: {
            terminal_log.text = terminal_log.text + revision + "\n> " + build + "\n> ";
        }
        onVolume: {
            status.volume = level;
            // update picture
        }
    }

    Flickable {
        id: flickArea
        x: 561
        y: 28
        width: 250
        height: 64
        contentWidth: terminal_log.width;
        contentHeight: terminal_log.height
        flickableDirection: Flickable.VerticalFlick
        clip: true

        function ensureVisible(r)
        {
            if (terminal_log.paintedHeight > 64)
                contentY = terminal_log.paintedHeight - 64;
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
        x: 381
        y: 417
        color: "#ffffff"
        anchors.horizontalCenterOffset: 0
        anchors.horizontalCenter: parent.horizontalCenter
        font.pixelSize: 14
    }
}
