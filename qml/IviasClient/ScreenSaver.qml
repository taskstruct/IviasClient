import QtQuick 2.1

Image {
    id: screenSaver

    property bool locked: true

    source: "images/screensaver_bg.jpg"

    Item {
        id: unlockArea
        x: 722
        y: 233
        width: parent.width - x
        height: 300

        UnlockEffect {
            id: unlockEffect
            x: 25
            y: 25

            Image {
                source: "images/padlock.png"

                anchors.centerIn: parent
            }
        }

        MouseArea {
            width: 300
            height: 300

            onClicked: screenSaver.locked = false
        }

        Text {
            text: "Touch to unlock"

            color: "white"

            font.pointSize: 28

            anchors.left: unlockEffect.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 40
        }
    }
}
