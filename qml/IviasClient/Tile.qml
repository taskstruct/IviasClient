import QtQuick 2.1
import IviasClient 1.0

Item {
    id: container

    property int page: 0
    property bool flipped: false

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

            Item {
                id: tileContent

                clip: true

                anchors.top: frontView.top
                anchors.left: frontView.left
                anchors.bottom: titleBackground.top
                anchors.right: frontView.right
                anchors.margins: 5

                Image {
                    source: thumbnail
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectFit
                }
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

                //TODO
                text: title

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
                        contentLoder.source = adsModel.directory + "Ad" + index * adsModel.page * 6 + "/main.qml"
                        console.debug("Fron clicked")
                        ClickCounter.increment(container.page, index);
                    }
                }
            }
        }

        back: Item {
            id: backItem

            anchors.fill: parent

            Item {
                id: bar
                height: 50

                width: parent.width
                anchors.top: parent.top

                Rectangle {
                    color: "#dbe3e2"
                    anchors.fill: parent
                }

                Text {

                    text: title

                    color: "#374845"
                    font.family: "Ubuntu"
                    font.pointSize: 20


                    anchors.left: backButton.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 5
                }

                Image {
                    id: backButton
                    source: "images/back.png"

                    anchors.left: parent.left
                    anchors.rightMargin: 5
                    anchors.verticalCenter: parent.verticalCenter

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            clickedAnim.start()
                            contentLoder.item.goBackAction()
                        }
                    }

                    SequentialAnimation on scale {
                        id: clickedAnim
                        running: false
                        NumberAnimation {
                            to: 0.9
                            duration: 100
                        }
                        NumberAnimation {
                            to: 1.1
                            duration: 100
                        }
                        NumberAnimation {
                            to: 0.9
                            duration: 100
                        }
                        NumberAnimation {
                            to: 1
                            duration: 100
                        }
                    }
                }
            }

            Loader {
                id: contentLoder

                width: parent.width
                clip: true
//                source: startpage

                anchors.top: bar.bottom
                anchors.bottom: parent.bottom

                onLoaded: {
                    container.flipped = true
                }

                Connections {
                    target: contentLoder.item

                    onNeedToClose: {
                        container.flipped = false
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
            }

            onRunningChanged: {
                if( !running ) {
                    contentLoder.source = ""
                }
            }
        }]
    }
}
