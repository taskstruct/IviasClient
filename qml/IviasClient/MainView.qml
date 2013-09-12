import QtQuick 2.0
import IviasClient 1.0

Image {
    id: mainView

    anchors.fill: parent

    property AnimatedContainer focusedAnimCont: animContainer1
    property AnimatedContainer bufferedAnimCont: animContainer2
    property TitleItem selectedTitle: title11
    property int currentIndex: 0

    property int __itemsPerRow: 3

    source: "images/mainview_bg.jpg"

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

    Item {
        id: viewsTitlesContainerTop

        width: 1200
        height: 60

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 20

        Row {
            id: titlesRowTop
            spacing: 10

            width: 1200
            height: 60

            TitleItem {
                id: title11
                text: IviasSettings.titles[0]
                selected: true

                onClicked: selectAdsGrid( 0, title11 )
            }

            TitleItem {
                id: title12
                text: IviasSettings.titles[1]

                onClicked: selectAdsGrid( 1, title12 )
            }

            TitleItem {
                id: title13
                text: IviasSettings.titles[2]

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

        width: mainView.width
        height: 475

        anchors.top: viewsTitlesContainerTop.bottom
        anchors.topMargin: 10

        AnimatedContainer {
            id: animContainer1

            adsPage: 0
        }

        AnimatedContainer {
            id: animContainer2
        }
    }

    MirrorEffect {
        anchors.top:  viewsContainer.bottom
        anchors.topMargin: -50
        mirroredItem: viewsContainer

        width: viewsContainer.width
        height: 150
    }

    Item {
        id: viewsTitlesContainerBottom

        width: 1200
        height: 60

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20

        Row {
            id: titlesRowBottom
            spacing: 10

            width: 1200
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

    Image {
        source: "images/padlock.png"

        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10

        MouseArea {
            anchors.fill: parent

            onClicked: window.screenLocked = true
        }
    }
}
