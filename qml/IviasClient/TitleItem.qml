import QtQuick 2.1
import QtQuick.Layouts 1.1

Item {
    id: viewTitle

    property alias text: title.text
    property bool selected: false

    property int indicatorDirection: 0

    signal clicked

    Layout.fillWidth: true
    Layout.fillHeight: true

    onIndicatorDirectionChanged: {
        if( indicatorDirection == 0 ) {
            selectIndicator.anchors.top = viewTitle.top
            selectIndicator.anchors.bottom = undefined
        }
        else {
            selectIndicator.anchors.top = undefined
            selectIndicator.anchors.bottom = viewTitle.bottom
        }
    }

    Text {
        id: title
        text: ""
        font.family: "Ubuntu"
        font.pixelSize: viewTitle.height * 0.5
        color: viewTitle.selected ? "#2f6e7d" : "#dbe3e2"
        style: viewTitle.selected ? Text.Sunken : Text.Normal

        anchors.centerIn: parent
    }

    ShaderEffectSource {
        id: effectBuffer
        anchors.fill: parent
        sourceItem: title
        live: false
        visible: false
    }

    Rectangle {
        anchors.fill: parent
        color: "#3c7977"
        opacity: 0.5
    }

    Rectangle {
        id: selectIndicator
        width: parent.width
        height: 5
        color: "#85a894"
//        opacity: 0.5
        visible: viewTitle.selected
    }

    MouseArea {
        anchors.fill: parent

        onClicked: {
            if( !viewTitle.selected )
            {
                viewTitle.selected = true
                viewTitle.clicked()
            }
        }
    }

    states: [
        State {
            name: "selected"
            when: selected
        }
    ]

    transitions: [
        Transition {
            from: ""
            to: "selected"

            SequentialAnimation {
                id: fadeoutAnimation

                PropertyAction { target: effectBuffer; property: "visible"; value: true }

                ParallelAnimation {
                    PropertyAnimation {
                        target: effectBuffer
                        property: "scale"
                        from: 1.0
                        to: 1.20
                        duration: 150
                    }
                    PropertyAnimation {
                        target: effectBuffer
                        property: "opacity"
                        from: 1.0
                        to: 0.0
                        duration: 150
                    }
                    ScriptAction { script: title.font.bold = true }
                }

                PropertyAction { target: effectBuffer; property: "visible"; value: false }
            }
        },

        Transition {
            from: "selected"
            to: ""

            ScriptAction { script: title.font.bold = false }
            PropertyAction { target: effectBuffer; property: "visible"; value: false }
        }
    ]
}
