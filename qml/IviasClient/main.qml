import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1
import IviasClient 1.0

Window {
    id: window

    width: 800
    height: 600

    property alias initText: initScrText.text
    property alias screenLocked: screenSaver.locked
    property bool initialized: false

    Component.onCompleted: console.debug("onCompleted: " + initialized)

    onInitializedChanged: console.debug("initialized: " + initialized)

    Rectangle {
        id: initialScreen
        color: "black"

        anchors.fill: parent

        Text {
            id: initScrText
            color: "white"

            text: "Initializing...<br>"
            textFormat: Text.RichText

            anchors.fill: parent
            anchors.margins: 10
        }
    }

    ScreenSaver {
        id: screenSaver

        locked: true
        enabled: false
        visible: false

        anchors.fill: parent
    }

    MainView {
        id: mainView
        enabled: false
        visible: false
    }

    Rectangle {
        id: seatbeltWarning

        property bool active: false

        color: "black"

        enabled: false
        visible: false

        anchors.fill: parent

        RowLayout {
            anchors.centerIn: parent

            Image {
                source: "images/warning.png"
            }

            Text {
                color: "white"
                text: "Please, put your seat belt!"
                font.pointSize: 36

                Layout.fillWidth: true
            }
        }

        Timer {
            interval: IviasSettings.seatbeltWarningDuration

            running: seatbeltWarning.active

            onTriggered: {
                seatbeltWarning.active = false;
            }
        }
    }

    Item {
        id: controller

        onStateChanged: console.debug("State: " + state)

        states: [
            State {
                name: "ScreenLocked"
                when: screenSaver.locked && !(seatbeltWarning.active && IviasSettings.showSeatbeltWarningEnabled) && window.initialized

                // rise up screensaver
                PropertyChanges {
                    target: screenSaver
                    enabled: true
                    visible: true
                    z: 1
                }
            },

            State {
                name: "Unlocked"
                when: !screenSaver.locked && !(seatbeltWarning.active && IviasSettings.showSeatbeltWarningEnabled) && window.initialized

                PropertyChanges {
                    target: mainView
                    enabled: true
                    visible: true
                    z:1
                }
            },

            State {
                name: "SeatbeltWarning"
                when: seatbeltWarning.active && IviasSettings.showSeatbeltWarningEnabled && window.initialized

                PropertyChanges {
                    target: seatbeltWarning
                    enabled: true
                    visible: true
                    z:1
                }
            }

        ]

        transitions: [
            Transition {
                from: ""
                to: "ScreenLocked"

                SequentialAnimation {

                    PropertyAction {
                        target: screenSaver
                        property: "opacity"
                        value: 0
                    }

                    PauseAnimation {
                        duration: 750
                    }

                    NumberAnimation {
                        target: screenSaver
                        property: "opacity"
                        from: 0; to: 1
                        duration: 250
                    }
                }
            },

            Transition {
                from: "Unlocked"
                to: "ScreenLocked"

                SequentialAnimation {
                    NumberAnimation {
                        target: screenSaver
                        property: "opacity"
                        from: 0; to: 1
                        duration: 500
                    }

                    PropertyAction {
                        target: mainView
                        property: "visible"
                    }
                }
            },

            Transition {
                from: "ScreenLocked"
                to: "Unlocked"

                SequentialAnimation {
                    NumberAnimation {
                        target: mainView
                        property: "opacity"
                        from: 0; to: 1
                        duration: 500
                    }

                    PropertyAction {
                        target: screenSaver
                        property: "visible"
                    }
                }
            },

            Transition {
                from: "*"
                to: "SeatbeltWarning"

                SequentialAnimation {
                    ParallelAnimation {
                        NumberAnimation {
                            target: seatbeltWarning
                            property: "scale"
                            from: 0.0; to: 1.0
                            duration: 200
                        }

                        NumberAnimation {
                            target: seatbeltWarning
                            property: "opacity"
                            from: 0; to: 1
                            duration: 200
                        }
                    }

                    PropertyAction {
                        targets: [screenSaver, mainView]
                        property: "visible"
                    }
                }
            },

            Transition {
                from: "SeatbeltWarning"
                to: "*"

                SequentialAnimation {
                    ParentAnimation {
                        NumberAnimation {
                            target: seatbeltWarning
                            property: "scale"
                            from: 1.0; to: 0.0
                            duration: 200
                        }

                        NumberAnimation {
                            target: seatbeltWarning
                            property: "opacity"
                            from: 1; to: 0
                            duration: 200
                        }
                    }

                    PropertyAction {
                        target: seatbeltWarning
                        property: "visible"
                    }

                    PropertyAction {
                        targets: [screenSaver, mainView, seatbeltWarning];
                        property: "z"
                    }
                }
            }
        ]
    }

    // for dubug
    Action {
        shortcut: "Ctrl+Q"
        onTriggered: Qt.quit()
    }

    Action {
        shortcut: "Esc"
        onTriggered: Qt.quit()
    }

    // locks screen
    Action {
        shortcut: "Ctrl+L"
        onTriggered: screenSaver.locked = true
    }

    // shows seatbelt warning
    Action {
        shortcut: "Ctrl+W"
        onTriggered: seatbeltWarning.active = !seatbeltWarning.active
    }
}
