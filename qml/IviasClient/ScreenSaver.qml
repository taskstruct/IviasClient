import QtQuick 2.2
import QtQml 2.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

LinearGradient {
    id: screenSaver

    property bool locked: true

    cached: true
    start: Qt.point( 0, height )
    end: Qt.point( width, 0 )

    gradient: Gradient {
        GradientStop { position: 0.0; color: "#2f6e7d" }
        GradientStop { position: 0.5; color: "#85a894" }
        GradientStop { position: 1.0; color: "#3c7977" }
    }

    Text {
        id: watermark

        color: "#3c7977"
        font.pointSize: 120
        font.bold: true
        style: Text.Sunken

        text: "IviasClient"
        opacity: 0.3

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: unlockEffect.top
        anchors.bottomMargin: -20
    }

    Glow {
        anchors.fill: watermark
        radius: 16
        samples: 16
        color: "white"
        source: watermark
        opacity: 0.3
    }

    UnlockEffect {
        id: unlockEffect

        anchors.centerIn: parent

        Image {
            source: "images/padlock.png"

            anchors.centerIn: parent
        }

        MouseArea {
            width: 300
            height: 300

            anchors.fill: parent

            onClicked: screenSaver.locked = false
        }
    }

    Text {
        text: "Touch to unlock"

        color: "white"

        font.pointSize: 28

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: unlockEffect.bottom
        anchors.bottomMargin: 20
    }
}
