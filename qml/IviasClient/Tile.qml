import QtQuick 2.1
import QtWebKit 3.0

Item {
    id: container

    property alias title: titleText.text
    property alias content: tileContent.source
    property alias contentComponent: tileContent.sourceComponent
    property string adUrl: ""

    property url backSource: ""
    property Component backComponent: null

    property bool flipped: false

    property var __backContent: null

    Flipable {
        id: tile

        width: parent.width
        height: parent.height

        front: Item {
            id: frontView

            anchors.fill: tile

            Rectangle {
                id: frontViewBackground

                color: "#dbe3e2"
                opacity: 0.5

                anchors.fill: parent
            }

            // todo: put loader in the end to be over mouse area?
            Loader {
                id: tileContent

                anchors.top: frontView.top
                anchors.left: frontView.left
                anchors.bottom: titleBackground.top
                anchors.right: frontView.right
                anchors.margins: 5

                clip: true
            }

            Rectangle {
                id: titleBackground

                width: parent.width
                height: 30

                color: "#dbe3e2"
                opacity: 0.5

                anchors.bottom: parent.bottom
            }

            Text {
                id: titleText

                font.family: "Ubuntu"
                font.pointSize: 20

                color: "#374845"

                anchors.left: titleBackground.left
                anchors.verticalCenter: titleBackground.verticalCenter
                anchors.leftMargin: 5
            }

            MouseArea {

                anchors.fill: parent

                onClicked: {
                    if( !container.flipped )
                    {
                        container.flipped = true
                        console.debug("Fron clicked")
                    }
                }
            }
        }

        back: Item {
            id: backItem

            anchors.fill: parent
            anchors.margins: 10

            WebView {
                id: webView
                anchors.fill: parent
            }

            Image {
                source: "images/close.png"

                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: -5

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        container.flipped = !container.flipped
                        clickedAnim.start()
                    }
                }

                SequentialAnimation on scale {
                    id: clickedAnim
                    running: false
                    NumberAnimation {
                        to: 0.8
                        duration: 100
                    }
                    NumberAnimation {
                        to: 1.2
                        duration: 100
                    }
                    NumberAnimation {
                        to: 0.8
                        duration: 100
                    }
                    NumberAnimation {
                        to: 1
                        duration: 100
                    }
                }
            }
        }

        transform: Rotation {
            id: rotation
            origin.x: tile.width/2
            origin.y: tile.height/2
            axis.x: 1; axis.y: 0; axis.z: 0     // set axis.y to 1 to rotate around y-axis
            angle: 0    // the default angle
        }

        states: State {
            name: "back"
            when: container.flipped

            PropertyChanges {
                target: webView
                url: adUrl
            }

            PropertyChanges { target: rotation; angle: 180 }

            ParentChange {
                target: tile;
                parent: mainView;
                x: 0; y: 0;
                width: mainView.width
                height: mainView.height
            }

            PropertyChanges {
                target: tile.front
                enabled: false
            }

            PropertyChanges {
                target: container.parent
                opacity: 0
            }
        }

        transitions: [ Transition {
            from: ""
            to: "back"

            ParentAnimation { 
                NumberAnimation { properties: "x,y,width,height"; duration: 500 }
            }

            NumberAnimation { target: rotation; property: "angle"; duration: 500 }

            NumberAnimation { target: container.parent; property: "opacity"; duration: 500 }
        },

        Transition {
            from: "back"
            to: ""

            SequentialAnimation {
                PauseAnimation { duration: 300 }
                ParallelAnimation {
                    ParentAnimation {
                        NumberAnimation { properties: "x,y,width,height"; duration: 500 }
                    }

                    NumberAnimation { target: rotation; property: "angle"; duration: 500 }

                    NumberAnimation { target: container.parent; property: "opacity"; duration: 500 }
                }

                ScriptAction {
                    script: webView.url = ""
                }
            }
        }]
    }
}
