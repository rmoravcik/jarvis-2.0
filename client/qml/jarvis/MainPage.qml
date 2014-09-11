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
        id: battery_timer
        interval: 30000
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

        property int index: 0

        function check_configuration() {
            if (index == 0) {
                bluetooth.getVersion();
                index++;
            } else if (index == 1) {
                bluetooth.getBattery();
                index++;
            } else if (index == 2) {
                bluetooth.getHelmet();
                index++;
            } else if (index == 3) {
                bluetooth.getRepulsors();
                index++
            } else if (index == 4) {
                bluetooth.getUnibeam();
                index++
            }
        }
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

        function open() {
            helmet_open.start();
        }

        function close() {
            if (helmet.visible == false)
                helmet.visible = true;
            helmet_close.start();
        }

        NumberAnimation on opacity {
            id: helmet_open
            from: 1
            to: 0
        }

        NumberAnimation on opacity {
            id: helmet_close
            from: 0
            to: 1
        }
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

        function off() {
            unibeam_off.start();
        }

        function on() {
            if (unibeam.visible == false)
                unibeam.visible = true;
            unibeam_on.start();
        }

        NumberAnimation on opacity {
            id: unibeam_off
            from: 1
            to: 0
        }

        NumberAnimation on opacity {
            id: unibeam_on
            from: 0
            to: 1
        }
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
        x: 367
        y: 35
        width: 120
        height: 180

        MouseArea {
           id: helmet_button
           anchors.fill: parent
           onClicked: {
               if (bluetooth.isConnected()) {
                   if (status.helmet == Bluetooth.HelmetClose) {
                       helmet.open();
                       bluetooth.setHelmet(Bluetooth.HelmetOpen);
                   } else {
                       helmet.close();
                       bluetooth.setHelmet(Bluetooth.HelmetClose);
                   }
                   enabled = false;
               }
           }
       }
    }

    Item {
        x: 367
        y: 285
        width: 120
        height: 120

        MouseArea {
           id: unibeam_button
           anchors.fill: parent
           onClicked: {
               if (bluetooth.isConnected()) {
                   if (status.unibeam == Bluetooth.PowerOn) {
                       unibeam.off();
                       bluetooth.setUnibeam(Bluetooth.PowerOff);
                   } else {
                       unibeam.on();
                       bluetooth.setUnibeam(Bluetooth.PowerOn);
                   }
                   enabled = false;
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

    Item {
        x: 0
        y: 0
        width: 50
        height: 50

        MouseArea {
           id: reboot_button
           anchors.fill: parent
           onDoubleClicked: {
               if (bluetooth.isConnected()) {
                   battery_timer.stop();
                   battery_timer.start();
                   bluetooth.reboot();
               }
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
            status.check_configuration();
            battery_timer.start();
            terminal_log.text = terminal_log.text + "Connected!\n> ";
            connect_button.visible = false;
            system.visible = true;
            system_connected.start();
        }
        onDisconnected: {
            battery_timer.stop();
            terminal_log.text = terminal_log.text + "Disconnected!\n> ";
        }
        onBattery: {
            battery.text = "Current power level is at " + capacity + "% and holding steady.";
            status.check_configuration();
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
                if (helmet.opacity == 0 || helmet.visible == false)
                    helmet.close();
            } else {
                if (helmet.opacity == 1)
                    helmet.open();
            }
            status.check_configuration();

            if (helmet_button.enabled == false)
                helmet_button.enabled = true;
        }
        onIntensity: {
            status.intensity[device] = level;
            // update picture
        }
        onRepulsorBlastGenerated: {
            // fixme
        }
        onRepulsors: {
            status.repulsors = state;
            // update picture
            status.check_configuration();
        }
        onUnibeam: {
            status.unibeam = state;

            if (status.unibeam == Bluetooth.PowerOn) {
                if (unibeam.opacity == 0 || unibeam.visible == false)
                    unibeam.on();
            } else {
                if (unibeam.opacity == 1)
                    unibeam.off();
            }

            if (unibeam_button.enabled == false)
                unibeam_button.enabled = true;
        }
        onVersion: {
            terminal_log.text = terminal_log.text + revision + "\n> " + build + "\n> ";
            status.check_configuration();
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
