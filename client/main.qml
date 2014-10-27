import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQml 2.2
import QtMultimedia 5.0

import Bluetooth 1.0

ApplicationWindow {
    id: main_window
    visible: true
    width: 800
    height: 480
    title: qsTr("J.A.R.V.I.S.")

    FontLoader {
        id: consoleFont
        source: "resources/gligoth.TTF"
    }

    Item {
        id: status
        property int eyes: Bluetooth.PowerOff
        property int helmet: Bluetooth.HelmetOpen
        property int repulsors: Bluetooth.PowerOff
        property int unibeam: Bluetooth.PowerOff

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
            } else if (index == 5) {
                bluetooth.getIntensity(Bluetooth.DeviceEyes);
                index++
            } else if (index == 6) {
                bluetooth.getIntensity(Bluetooth.DeviceRepulsors);
                index++
            } else if (index == 7) {
                bluetooth.getIntensity(Bluetooth.DeviceUnibeam);
                index++
            } else if (index == 8) {
                bluetooth.getVolume();
                index++
            }
        }
    }

    Bluetooth {
        id: bluetooth

        onConnected: {
            status.check_configuration();
            battery_timer.start();
            terminal_log.text = terminal_log.text + "Connected...\n> ";
            connect_button.visible = false;
            suit_diagnostics.on();
        }

        onDisconnected: {
            battery_timer.stop();
            suit_diagnostics.off();
            terminal_log.text = terminal_log.text + "Disconnected...\n> ";
        }

        onConnectionError: {
            battery_timer.stop();
            suit_diagnostics.off();
            terminal_log.text = terminal_log.text + "Error...\n> ";
        }

        onReadTimetout: {
            terminal_log.text = terminal_log.text + "Timeout...\n> ";
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
            if (device == Bluetooth.DeviceEyes) {
                eyes_intensity_slider.value = level;
            } else if (device == Bluetooth.DeviceRepulsors) {
                repulsors_intensity_slider.value = level;
            } else if (device == Bluetooth.DeviceUnibeam) {
                unibeam_intensity_slider.value = level;
            }

            status.check_configuration();
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

            status.check_configuration();
        }

        onVersion: {
            terminal_log.text = terminal_log.text + revision + "\n> " + build + "\n> ";
            status.check_configuration();
        }

        onVolume: {
            volume_slider.value = level;
        }
    }

    SoundEffect {
            id: clickEffect
            source: "resources/click.wav"
    }

    Item {
        id: main_page
        x: 0
        y: 0
        width: 800
        height: 480
        rotation: 0
        state: "MAIN_WINDOW"

        Image {
            anchors.fill: parent
            source: "resources/background.png"
        }

        states: [
            State {
                name: "MAIN_WINDOW"

                PropertyChanges {
                    target: options_page
                    visible: false
                }

                PropertyChanges {
                    target: options_page_button
                    enabled: false
                }

                PropertyChanges {
                    target: jarvis_button
                    enabled: true
                }

                PropertyChanges {
                    target: helmet_button
                    enabled: true
                }

                PropertyChanges {
                    target: unibeam_button
                    enabled: true
                }

                PropertyChanges {
                    target: suit_button
                    enabled: true
                }

                PropertyChanges {
                    target: suit_diagnostics_button
                    enabled: true
                }

                PropertyChanges {
                    target: reactor_button
                    enabled: true
                }
            },
            State {
                name: "OPTIONS_WINDOW"

                PropertyChanges {
                    target: options_page
                    visible: true
                }

                PropertyChanges {
                    target: options_page_button
                    enabled: true
                }

                PropertyChanges {
                    target: jarvis_button
                    enabled: false
                }

                PropertyChanges {
                    target: helmet_button
                    enabled: false
                }

                PropertyChanges {
                    target: unibeam_button
                    enabled: false
                }

                PropertyChanges {
                    target: suit_button
                    enabled: false
                }

                PropertyChanges {
                    target: suit_diagnostics_button
                    enabled: false
                }

                PropertyChanges {
                    target: reactor_button
                    enabled: false
                }
            }
        ]

        transitions: [
            Transition {
                from: "MAIN_WINDOW"
                to: "OPTIONS_WINDOW"

                NumberAnimation {
                    target: options_page
                    properties: "opacity"
                    from: 0
                    to: 1
                    duration: 500
                }
            },
            Transition {
                from: "OPTIONS_WINDOW"
                to: "MAIN_WINDOW"

                NumberAnimation {
                    target: options_page
                    properties: "opacity"
                    from: 1
                    to: 0
                    duration: 500
                }
            }
        ]

        Item {
            id: jarvis
            x: 32
            y: 271
            width: 140
            height: 140

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
                        clickEffect.play();
                        bluetooth.playQuote();
                    }
                }
            }
        }

        Item {
            id: helmet
            x: 313
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
                           clickEffect.play();
                           helmet_image.open();
                           bluetooth.setHelmet(Bluetooth.HelmetOpen);
                       } else {
                           clickEffect.play();
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
            x: 313
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
                            clickEffect.play();
                            bluetooth.setUnibeam(Bluetooth.PowerOff);
                        } else {
                            unibeam_image.on();
                            clickEffect.play();
                            bluetooth.setUnibeam(Bluetooth.PowerOn);
                        }
                        enabled = false;
                    }
                }
            }
        }

        Item {
            id: suit
            x: 623
            y: 119
            width: 128
            height: 128

            MouseArea {
                id: suit_button
                anchors.fill: parent

                onClicked: {
                    suit_diagnostics.on();
                }
            }
        }

        Flipable {
            id: suit_diagnostics
            x: 570
            y: 291
            width: 206
            height: 160
            visible: false

            Timer {
                id: suit_diagnostics_timer
                interval: 5000
                repeat: true

                onTriggered: {
                    if (rot.angle == 0) {
                        rot.angle = 180;
                    } else {
                        rot.angle = 0;
                    }
                }
            }

            function off() {
                suit_diagnostics_timer.stop();
                suit_diagnostics_off.start();
                visible = false;
            }

            function on() {
                if (visible == false)
                    visible = true;
                suit_diagnostics_on.start();
                suit_diagnostics_timer.start();
            }

            NumberAnimation on opacity {
                id: suit_diagnostics_off
                from: 1
                to: 0
            }

            NumberAnimation on opacity {
                id: suit_diagnostics_on
                from: 0
                to: 1
            }

            transform: Rotation {
                id: rot

                origin {
                    x: 103
                    y: 80
                }

                axis {
                    x: 0
                    y: 1
                    z: 0
                }

                angle: 0

                Behavior on angle {
                    PropertyAnimation{}
                }
            }

            front: Image {
                anchors.centerIn: parent
                source: "resources/suit_diagnostics.png"
            }

            back: Image {
                anchors.centerIn: parent
                source: "resources/suit_diagnostics2.png"
            }

            MouseArea {
                id: suit_diagnostics_button
                anchors.fill: parent

                onClicked: {
                    main_page.state = "OPTIONS_WINDOW";
                }
            }
        }

        Item {
            id: reactor
            x: 97
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
                   } else {
                       if (bluetooth.isConnected()) {
                           battery_timer.stop();
                           battery_timer.start();
                           terminal_log.text = terminal_log.text + "Rebooting...\n> ";
                           clickEffect.play();
                           bluetooth.reboot();
                       }
                   }
               }
           }
        }

        Item {
            id: battery
            y: 426

            anchors {
                horizontalCenterOffset: -27
                horizontalCenter: parent.horizontalCenter
            }

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
                x: 0
                y: 0
                color: "#ffffff"
                text: "Current power level is at 100% and holding steady."

                anchors {
                    verticalCenter: parent.verticalCenter
                    horizontalCenter: parent.horizontalCenter
                }

                horizontalAlignment: Text.AlignHCenter

                font {
                    family: consoleFont.name
                    pixelSize: 16
                }
            }
        }

        Flickable {
            id: flick
            x: 508
            y: 28
            width: 250
            height: 64
            contentWidth: terminal_log.width
            contentHeight: terminal_log.height
            flickableDirection: Flickable.VerticalFlick
            clip: true

            function ensureVisible(r) {
                if (terminal_log.paintedHeight > 64)
                    contentY = terminal_log.paintedHeight - 64;
            }

            TextEdit {
                id: terminal_log
                width: flick.width
                height: flick.height
                color: "#ffffff"
                text: qsTr("> Disconnected...\n> ")
                activeFocusOnPress: false
                readOnly: true

                font {
                    family: consoleFont.name
                    pixelSize: 14
                }

                onCursorRectangleChanged: {
                    flick.ensureVisible(cursorRectangle);
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
            anchors.horizontalCenterOffset: -27
            iconSource: ""

            style: ButtonStyle {
                label: Text {
                    renderType: Text.NativeRendering
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter

                    font {
                        family: consoleFont.name
                        pixelSize: 22
                    }

                    text: control.text
                }
            }

            onClicked: {
                terminal_log.text = terminal_log.text + "Connecting...\n> ";
                clickEffect.play();
                bluetooth.requestConnection();
            }
        }
    }

    Item {
        id: options_page
        x: 187
        y: 0
        width: 426
        height: 480
        rotation: 0

        MouseArea {
            id: options_page_button
            x: -187
            y: 0
            width: 800
            height: 480

            onClicked: {
                main_page.state = "MAIN_WINDOW";
            }
        }

        Image {
            anchors.fill: parent
            source: "resources/options.png"
        }

        Text {
            x: 65
            y: 87
            color: "#ffffff"

            font {
                family: consoleFont.name
                pixelSize: 16
            }

            text: qsTr("VOLUME")
        }

        Slider {
            id: volume_slider
            x: 65
            y: 125
            width: 312
            height: 28
            minimumValue: 0
            maximumValue: 7
            stepSize: 1.0
            value: 0

            onValueChanged: {
                bluetooth.setVolume(value);

            }
        }

        Text {
            x: 65
            y: 185
            color: "#ffffff"

            font {
                family: consoleFont.name
                pixelSize: 16
            }

            text: qsTr("EYES")
        }

        Slider {
            id: eyes_intensity_slider
            x: 65
            y: 223
            width: 312
            height: 28
            minimumValue: 0
            maximumValue: 9
            stepSize: 1.0
            value: 0

            onValueChanged: {
                bluetooth.setIntensity(Bluetooth.DeviceEyes, value);
            }
        }

        Text {
            x: 65
            y: 271
            color: "#ffffff"

            font {
                family: consoleFont.name
                pixelSize: 16
            }

            text: qsTr("REPULSORS")
        }

        Slider {
            id: repulsors_intensity_slider
            x: 65
            y: 309
            width: 312
            height: 28
            minimumValue: 0
            maximumValue: 9
            stepSize: 1.0
            value: 0

            onValueChanged: {
                bluetooth.setIntensity(Bluetooth.DeviceRepulsors, value);
            }
        }

        Text {
            x: 65
            y: 357
            color: "#ffffff"
            font.family: consoleFont.name
            font.pixelSize: 16
            text: qsTr("UNIBEAM")
        }

        Slider {
            id: unibeam_intensity_slider
            x: 65
            y: 395
            width: 312
            height: 28
            minimumValue: 0
            maximumValue: 9
            stepSize: 1.0
            value: 0

            onValueChanged: {
                bluetooth.setIntensity(Bluetooth.DeviceUnibeam, value);
            }
        }
    }
}
