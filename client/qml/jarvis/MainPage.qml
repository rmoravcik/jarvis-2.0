import QtQuick 1.1
import com.nokia.meego 1.0
import Bluetooth 1.0

Page {
    id: main
    width: 854
    height: 480
    tools: null
    orientationLock: PageOrientation.LockLandscape

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

    Item {
        id: jarvis
        x: 92
        y: 277
        width: 128
        height: 128

        Image {
            id: jarvis_image
            anchors.fill: parent
            visible: false
            source: "resources/jarvis.png"

            function hide() {
                jarvis_show.stop();
                jarvis_hide.start();
                jarvis_image.visible = false;
            }

            function show() {
                jarvis_image.visible = true;
                jarvis_show.start();
            }

            NumberAnimation on opacity {
                id: jarvis_hide
                from: 1
                to: 0
            }

            NumberAnimation on opacity {
                id: jarvis_show
                loops: Animation.Infinite
                easing.type: Easing.OutInBounce
                from: 0
                to: 1
                duration: 500
            }
        }

        MouseArea {
            id: jarvis_button
            anchors.fill: parent

            onClicked: {
                if (bluetooth.isConnected()) {
                    jarvis_image.show();
                    bluetooth.playQuote();
                }
            }
        }
    }

    Item {
        id: helmet
        x: 367
        y: 35
        width: 120
        height: 180

        Image {
            id: helmet_image
            anchors.fill: parent
            visible: false
            source: "resources/helmet.png"

            function open() {
                helmet_open.start();
            }

            function close() {
                if (helmet_image.visible == false)
                    helmet_image.visible = true;
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

        MouseArea {
           id: helmet_button
           anchors.fill: parent
           onClicked: {
               if (bluetooth.isConnected()) {
                   if (status.helmet == Bluetooth.HelmetClose) {
                       helmet_image.open();
                       bluetooth.setHelmet(Bluetooth.HelmetOpen);
                   } else {
                       helmet_image.close();
                       bluetooth.setHelmet(Bluetooth.HelmetClose);
                   }
                   enabled = false;
               }
           }
       }
    }

    Item {
        id: unibeam
        x: 367
        y: 285
        width: 120
        height: 120

        Image {
            id: unibeam_image
            visible: false
            source: "resources/unibeam.png"

            function off() {
                unibeam_off.start();
                unibeam_image.visible = false;
            }

            function on() {
                if (unibeam_image.visible == false)
                    unibeam_image.visible = true;
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

        MouseArea {
            id: unibeam_button
            anchors.fill: parent
            onClicked: {
                if (bluetooth.isConnected()) {
                    if (status.unibeam == Bluetooth.PowerOn) {
                        unibeam_image.off();
                        bluetooth.setUnibeam(Bluetooth.PowerOff);
                    } else {
                        unibeam_image.on();
                        bluetooth.setUnibeam(Bluetooth.PowerOn);
                    }
                    enabled = false;
                }
            }
        }
    }

    Item {
        id: suit
        x: 676
        y: 119
        width: 128
        height: 128

        MouseArea {
            id: suit_button
            anchors.fill: parent
            onClicked: {
//               if (bluetooth.isConnected()) {
                bluetooth.getVersion();
//               }
            }
        }
    }

    Item {
        id: suit_diagnostics

        Image {
            id: suit_diagnostics_blue
            x: 630
            y: 298
            width: 188
            height: 139
            visible: false
            source: "resources/suit_diagnostics.png"

            function hide() {
                suit_diagnostics_blue_hide.start();
                suit_diagnostics_blue.visible = false;
            }

            function show() {
                if (suit_diagnostics_blue.visible == false)
                    suit_diagnostics_blue.visible = true;
                suit_diagnostics_blue_show.start();
            }

            NumberAnimation on opacity {
                id: suit_diagnostics_blue_hide
                from: 1
                to: 0
            }

            NumberAnimation on opacity {
                id: suit_diagnostics_blue_show
                from: 0
                to: 1
            }
        }

        Image {
            id: suit_diagnostics_red
            x: 630
            y: 308
            width: 193
            height: 137
            visible: false
            source: "resources/suit_diagnostics2.png"

            function hide() {
                suit_diagnostics_red_hide.start();
                suit_diagnostics_red.visible = false;
            }

            function show() {
                if (suit_diagnostics_red.visible == false)
                    suit_diagnostics_red.visible = true;
                suit_diagnostics_red_show.start();
            }

            NumberAnimation on opacity {
                id: suit_diagnostics_red_hide
                from: 1
                to: 0
            }

            NumberAnimation on opacity {
                id: suit_diagnostics_red_show
                from: 0
                to: 1
            }
        }
    }

    Item {
        id: reactor
        x: 149
        y: 84
        width: 140
        height: 140

        Timer {
            id: reactor_button_timer
            interval: 2000

            onTriggered: {
                reactor_image.hide();
            }
        }

        Image {
            id: reactor_image
            anchors.fill: parent
            visible: false
            source: "resources/reactor.png"

            function hide() {
                reactor_hide.start();
                reactor_image.visible = false;
            }

            function show() {
                if (reactor_image.visible == false)
                    reactor_image.visible = true;
                reactor_show.start();
                reactor_button_timer.start();
            }

            NumberAnimation on opacity {
                id: reactor_hide
                from: 1
                to: 0
            }

            NumberAnimation on opacity {
                id: reactor_show
                from: 0
                to: 1
            }
        }

        MouseArea {
           id: reactor_button
           anchors.fill: parent

           onDoubleClicked: {
               if (reactor_image.visible == false) {
                   if (bluetooth.isConnected()) {
                       reactor_image.show();
                   }
               }

               if (reactor_image.visible == true) {
                   if (bluetooth.isConnected()) {
                       battery_timer.stop();
                       battery_timer.start();
                       bluetooth.reboot();
                   }
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
            terminal_log.text = terminal_log.text + "Connected...\n> ";
            connect_button.visible = false;
            suit_diagnostics_red_show.show();
        }

        onDisconnected: {
            battery_timer.stop();
            suit_diagnostics_red_show.hide();
            terminal_log.text = terminal_log.text + "Disconnected...\n> ";
        }

        onBattery: {
            battery_text.text = "Current power level is at " + capacity + "% and holding steady.";
            status.check_configuration();
        }

        onEyes: {
            status.eyes = state;

            if (status.eyes == Bluetooth.PowerOn) {
                helmet_image.visible = true;
            } else {
                helmet_image.visible = false;
            }
        }

        onQuoteFinished: {
            jarvis_image.hide();
        }

        onHelmet: {
            status.helmet = state;

            if (status.helmet == Bluetooth.HelmetClose) {
                if (helmet_image.visible == false)
                    helmet_image.close();
            } else {
                if (helmet_image.visible == true)
                    helmet_image.open();
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
                if (unibeam_image.visible == false)
                    unibeam_image.on();
            } else {
                if (unibeam_image.visible == true)
                    unibeam_image.off();
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

    Item {
        id: battery

        Timer {
            id: battery_timer
            interval: 30000
            repeat: true

            onTriggered: {
                bluetooth.getBattery();
            }
        }

        Text {
            id: battery_text
            x: 381
            y: 417
            color: "#ffffff"
            anchors.horizontalCenterOffset: 0
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 14
        }
    }

    Flickable {
        id: flick
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
            width: flick.width;
            height: flick.height;
            color: "#ffffff"
            text: qsTr("> Disconnected...\n> ")
            activeFocusOnPress: false
            readOnly: true
            font.pixelSize: 12
            onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)
        }
    }
}
