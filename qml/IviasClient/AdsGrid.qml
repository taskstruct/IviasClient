import QtQuick 2.1
import IviasClient 1.0

Grid {
    id: mainLayout

    property alias page: adsModel.page

    anchors.fill: parent

    columns: 3
    rows: 2
    spacing: 15

    Repeater {

        model: AdsModel {
            id: adsModel
            page: 0
        }

        delegate: Tile {
            id: modelDelegate
            width: mainLayout.width / 3
            height: 200

            title: title
            adUrl: startpage
            page: adsModel.page
            index: model.index

            //TODO: Move this as static part of Tile
            contentComponent:  Item {
                Image {
                    source: thumbnail
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectFit
                }
            }

            onFlippedChanged: {
                console.debug("delegate flipped")
            }
        }
    }
}

