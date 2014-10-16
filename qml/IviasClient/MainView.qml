import QtQuick 2.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import IviasClient 1.0

Image {
    id: mainView

    anchors.fill: parent

    property AnimatedContainer focusedAnimCont: animContainer1
    property AnimatedContainer bufferedAnimCont: animContainer2
    property TitleItem selectedTitle: title11
    property int currentIndex: 0

    property int __itemsPerRow: 3

    source: "images/background.png"
    fillMode: Image.Stretch

    Component.onCompleted: bufferedAnimCont.visible = false

    function selectAdsGrid( index, titleId )
    {
        if( index == currentIndex )
        {
            // currently selected title is clicked again. Do nothing
            return
        }

        bufferedAnimCont.adsPage = index

        var newRow = Math.floor( index / __itemsPerRow )
        var oldRow = Math.floor( currentIndex / __itemsPerRow )

        if( newRow == oldRow )
        {
            if( currentIndex < index )
            {
                bufferedAnimCont.setupRightIn()
                focusedAnimCont.setupLeftOut()
            }
            else if ( currentIndex > index )
            {
                bufferedAnimCont.setupLeftIn()
                focusedAnimCont.setupRightOut()
            }
        }
        else
        {
            if( newRow > oldRow )
            {
                bufferedAnimCont.setupBottomIn()
                focusedAnimCont.setupTopOut()
            }
            else
            {
                bufferedAnimCont.setupTopIn()
                focusedAnimCont.setupBottomOut()
            }
        }

        // start animations
        bufferedAnimCont.startAnim()
        focusedAnimCont.startAnim()

        // swap referances
        var tmp = focusedAnimCont
        focusedAnimCont = bufferedAnimCont
        bufferedAnimCont = tmp

        selectedTitle.selected = false
        selectedTitle = titleId

        currentIndex = index
    }

    ColumnLayout {

        width: parent.width - panel.width
        height: parent.height

        Item {
            id: viewsTitlesContainerTop

            height: 60

            Layout.fillWidth: true

            RowLayout {
                id: titlesRowTop
                spacing: 2

                width: parent.width
                height: 60

                TitleItem {
                    id: title11
                    text: IviasSettings.titles[0]
                    selected: true
                    indicatorDirection: 1

                    onClicked: selectAdsGrid( 0, title11 )
                }

                TitleItem {
                    id: title12
                    text: IviasSettings.titles[1]
                    indicatorDirection: 1

                    onClicked: selectAdsGrid( 1, title12 )
                }

                TitleItem {
                    id: title13
                    text: IviasSettings.titles[2]
                    indicatorDirection: 1

                    onClicked: selectAdsGrid( 2, title13 )
                }

                Component.onCompleted: {
                    var childrenCount = children.length
                    var widthForItems = width - spacing * ( childrenCount - 1 )
                    var widthPerChild = widthForItems / childrenCount

                    for( var i = 0; i < childrenCount; ++i )
                    {
                        children[i].width = widthPerChild
                    }
                }
            }
        }

        Item {
            id: viewsContainer

            height: mainView.height - 2 * ( viewsTitlesContainerTop.height + viewsTitlesContainerBottom.height )

            Layout.fillWidth: true
            Layout.fillHeight: true

            clip: true

            AnimatedContainer {
                id: animContainer1

                adsPage: 0
            }

            AnimatedContainer {
                id: animContainer2
            }
        }

        MirrorEffect {
            height: 150
            mirroredItem: viewsContainer

            Layout.fillWidth: true
        }

        Item {
            id: viewsTitlesContainerBottom

            height: 60

            Layout.fillWidth: true

            RowLayout {
                id: titlesRowBottom
                spacing: 2

                width: parent.width
                height: 60

                TitleItem {
                    id: title21
                    text: IviasSettings.titles[3]

                    onClicked: selectAdsGrid( 3, title21 )
                }

                TitleItem {
                    id: title22
                    text: IviasSettings.titles[4]

                    onClicked: selectAdsGrid( 4, title22 )
                }

                TitleItem {
                    id: title23
                    text: IviasSettings.titles[5]

                    onClicked: selectAdsGrid( 5, title23 )
                }

                Component.onCompleted: {
                    var childrenCount = children.length
                    var widthForItems = width - spacing * ( childrenCount - 1 )
                    var widthPerChild = widthForItems / childrenCount

                    for( var i = 0; i < childrenCount; ++i )
                    {
                        children[i].width = widthPerChild
                    }
                }
            }
        }
    }

    Rectangle {
        width: 3
        height: parent.height

        border.width: 1
        border.color: "#3c7977"

        gradient: Gradient {
            GradientStop { position: 0.0; color: "#2f6e7d" }
            GradientStop { position: 0.5; color: "#85a894" }
            GradientStop { position: 1.0; color: "#3c7977" }
        }

        anchors.right: panel.left
    }

    Panel {
        id: panel
        width: 250
        height: parent.height
        anchors.right: parent.right
    }
}
