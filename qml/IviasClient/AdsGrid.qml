import QtQuick 2.1
import IviasClient 1.0

Grid {
    id: mainLayout

    property alias page: model.page

    anchors.fill: parent

    columns: 3
    rows: 2
    spacing: 15

    Repeater {

        model: AdsModel {
            id: model
            page: 0
        }

        delegate: Tile {
            id: modelDelegate
            width: mainLayout.width / 3
            height: 200

            title: title

            adUrl: startpage

            contentComponent:  Item {
                Image {
                    source: thumbnail
                    anchors.centerIn: parent
                }
            }

            onFlippedChanged: {
                console.debug("delegate flipped")
            }
        }
    }
}

