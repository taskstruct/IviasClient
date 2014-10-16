import QtQuick 2.2
import QtQuick.Layouts 1.1
import IviasClient 1.0 as IC

ColumnLayout {

    Item {
        height: 60

        Layout.fillWidth: true

        Text {
            text: "Information"
            font.family: "Ubuntu"
            font.pixelSize: 30
            color: "#dbe3e2"
            anchors.centerIn: parent
        }

        Rectangle {
            anchors.fill: parent
            color: "#3c7977"
            opacity: 0.5
        }
    }

    Text {
        id: clock

        color: "#dbe3e2"
        font.pixelSize: 30
        horizontalAlignment: Text.AlignHCenter
        textFormat: Text.RichText

        Layout.fillWidth: true

        Timer {
            id: dateTimeTimer
            interval: 10000
            repeat: true
            running: true
            triggeredOnStart: true

            property var locale: Qt.locale()

            onTriggered: {
                var dateTime = new Date()

//                clock.text = dateTime.toLocaleTimeString( locale, Locale.ShortFormat )
                clock.text = "<b>" + dateTime.getHours() + "</b>:" + dateTime.getMinutes() //TODO: Show 2 digits after 0:0

                moreInfo.updateInfo( dateTime, locale )
            }
        }
    }

    Text {
        id: moreInfo

        color: "#dbe3e2"
        font.pixelSize: 20
        horizontalAlignment: Text.AlignHCenter

        Layout.fillWidth: true

        function updateInfo( dateTime, locale ) {
            moreInfo.text = "<b>" + dateTime.getDate() + ' ' + locale.monthName( dateTime.getMonth() , Locale.ShortFormat) + "</b>"

            if( IC.PowerManager.isOnBattery() || ( IC.PowerManager.battery < 100.0 ) ) {
                moreInfo.text += " | " + IC.PowerManager.battery + "%"
            }
        }
    }

    Connections {
        target: IC.PowerManager

        onBatteryValueChanged: {
            console.debug("onBatteryValueChanged!")
            moreInfo.updateInfo( new Date(), dateTimeTimer.locale )
        }

        onPowerSupplyPlugedIn: { moreInfo.updateInfo( new Date(), dateTimeTimer.locale ) }

        onPowerSupplyPlugedOut: { moreInfo.updateInfo( new Date(), dateTimeTimer.locale ) }
    }

    Item {
        id: spacer
        Layout.fillWidth: true
        Layout.fillHeight: true
    }


    Item {
        id: lockButton
        height: 60

        Layout.fillWidth: true

        //        anchors.bottom: parent.bottom

        Image {
            id: lockIcon
            width: lockButton.height * 0.50
            height: lockButton.height * 0.50

            source: "images/padlock.png"

            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 10
        }

        Text {
            text: "Lock"
            font.family: "Ubuntu"
            font.pixelSize: lockButton.height * 0.50
            color: "#dbe3e2"

            anchors.left: lockIcon.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 5
        }

        Rectangle {
            anchors.fill: parent
            color: "#3c7977"
            opacity: 0.5
        }

        MouseArea {
            anchors.fill: parent
            onClicked: window.screenLocked = true
        }
    }
}
