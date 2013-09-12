import QtQuick 2.1

Item {
    id: viewTitle
    width: 240
    height: 60

    property alias text: title.text
    property bool selected: false

    signal clicked

    Text {
        id: title
        text: ""
        font.family: "Ubuntu"
        font.pointSize: 40
        color: "#dbe3e2"

        anchors.centerIn: parent
    }

    ShaderEffectSource {
        id: effectBuffer
        anchors.fill: parent
        sourceItem: title
        visible: false
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
                        to: 1.5
                        duration: 200
                    }
                    PropertyAnimation {
                        target: effectBuffer
                        property: "opacity"
                        from: 1.0
                        to: 0.0
                        duration: 200
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
