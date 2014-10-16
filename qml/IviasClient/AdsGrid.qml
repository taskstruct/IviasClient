import QtQuick 2.2
import QtQuick.Layouts 1.1
import IviasClient 1.0

GridLayout {
    id: mainLayout

    property alias page: adsModel.page

    anchors.fill: parent

    columns: 3
    rows: 2
    columnSpacing: 15
    rowSpacing: 15

    Repeater {

        model: AdsModel {
            id: adsModel
            page: 0
        }

        delegate: Tile {
            id: modelDelegate

            Layout.fillWidth: true
            Layout.fillHeight: true

            page: adsModel.page

            onFlippedChanged: {
                console.debug("delegate flipped")
            }
        }
    }
}

