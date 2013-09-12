import QtQuick 2.1

Item {
    id: animContainer

    width: mainView.width - 300
    height: viewsContainer.height - 50

    anchors.horizontalCenter: parent.horizontalCenter
    anchors.top: parent.top
    anchors.topMargin: 10

    property alias adsPage: adsGrid.page
    property bool __isExitAnim: false
    property int __animDirection: Qt.Horizontal
    property int __horizontalAnimationDuration: 400
    property int __verticalAnimationDuration: (__horizontalAnimationDuration * height) / width

    function startAnim() {
        if(!__isExitAnim) {
            animContainer.visible = true
        }

        // animation durations
        if( Qt.Horizontal == __animDirection ) {
            rotAnim.duration = __horizontalAnimationDuration
            transAnim.duration = __horizontalAnimationDuration
            scaleAnim.duration = __horizontalAnimationDuration
        }
        else {
            rotAnim.duration = __verticalAnimationDuration
            transAnim.duration = __verticalAnimationDuration
            scaleAnim.duration = __verticalAnimationDuration
        }

        cubeRotAnim.start()
    }

    function setupTopIn() {
        // set initial values for transformations
        rotationTransformation.angle = 80
        rotationTransformation.axis = Qt.vector3d(1, 0, 0)
        translateTransformation.x = 0
        translateTransformation.y = -viewsContainer.height
        scaleTransformation.xScale = 0.5
        scaleTransformation.yScale = 0.5

        // set final values for animations
        rotAnim.to = 0
        transAnim.property = "y"
        transAnim.to = 0
        scaleAnim.to = 1
        __isExitAnim = false
        __animDirection = Qt.Vertical
    }

    function setupTopOut() {
        // set initial values for transformations
        rotationTransformation.angle = 0
        rotationTransformation.axis = Qt.vector3d(1, 0, 0)
        translateTransformation.x = 0
        translateTransformation.y = 0
        scaleTransformation.xScale = 1
        scaleTransformation.yScale = 1

        // set final values for animations
        rotAnim.to = 80
        transAnim.property = "y"
        transAnim.to = -viewsContainer.height
        scaleAnim.to = 0.5
        __isExitAnim = true
        __animDirection = Qt.Vertical
    }

    function setupBottomIn() {
        // set initial values for transformations
        rotationTransformation.angle = -80
        rotationTransformation.axis = Qt.vector3d(1, 0, 0)
        translateTransformation.x = 0
        translateTransformation.y = viewsContainer.height
        scaleTransformation.xScale = 0.5
        scaleTransformation.yScale = 0.5

        // set final values for animations
        rotAnim.to = 0
        transAnim.property = "y"
        transAnim.to = 0
        scaleAnim.to = 1
        __isExitAnim = false
        __animDirection = Qt.Vertical
    }

    function setupBottomOut() {
        // set initial values for transformations
        rotationTransformation.angle = 0
        rotationTransformation.axis = Qt.vector3d(1, 0, 0)
        translateTransformation.x = 0
        translateTransformation.y = 0
        scaleTransformation.xScale = 1
        scaleTransformation.yScale = 1

        // set final values for animations
        rotAnim.to = -80
        transAnim.property = "y"
        transAnim.to = viewsContainer.height
        scaleAnim.to = 0.5
        __isExitAnim = true
        __animDirection = Qt.Vertical
    }

    function setupLeftIn() {
        // set initial values for transformations
        rotationTransformation.angle = -80
        rotationTransformation.axis = Qt.vector3d(0, 1, 0)
        translateTransformation.x = -viewsContainer.width
        translateTransformation.y = 0
        scaleTransformation.xScale = 0.5
        scaleTransformation.yScale = 0.5

        // set final values for animations
        rotAnim.to = 0
        transAnim.property = "x"
        transAnim.to = 0
        scaleAnim.to = 1
        __isExitAnim = false
        __animDirection = Qt.Horizontal
    }

    function setupLeftOut() {
        // set initial values for transformations
        rotationTransformation.angle = 0
        rotationTransformation.axis = Qt.vector3d(0, 1, 0)
        translateTransformation.x = 0
        translateTransformation.y = 0
        scaleTransformation.xScale = 1
        scaleTransformation.yScale = 1

        // set final values for animations
        rotAnim.to = -80
        transAnim.property = "x"
        transAnim.to = -viewsContainer.width
        scaleAnim.to = 0.5
        __isExitAnim = true
        __animDirection = Qt.Horizontal
    }

    function setupRightIn() {
        // set initial values for transformations
        rotationTransformation.angle = 80
        rotationTransformation.axis = Qt.vector3d(0, 1, 0)
        translateTransformation.x = viewsContainer.width
        translateTransformation.y = 0
        scaleTransformation.xScale = 0.5
        scaleTransformation.yScale = 0.5

        // set final values for animations
        rotAnim.to = 0
        transAnim.property = "x"
        transAnim.to = 0
        scaleAnim.to = 1
        __isExitAnim = false
        __animDirection = Qt.Horizontal
    }

    function setupRightOut() {
        // set initial values for transformations
        rotationTransformation.angle = 0
        rotationTransformation.axis = Qt.vector3d(0, 1, 0)
        translateTransformation.x = 0
        translateTransformation.y = 0
        scaleTransformation.xScale = 1
        scaleTransformation.yScale = 1

        // set final values for animations
        rotAnim.to = 80
        transAnim.property = "x"
        transAnim.to = viewsContainer.width
        scaleAnim.to = 0.5
        __isExitAnim = true
        __animDirection = Qt.Horizontal
    }

    transform: [ Rotation
        {
            id: rotationTransformation

            axis: Qt.vector3d(1, 0, 0)

            origin.x: animContainer.width / 2.0
            origin.y: animContainer.height / 2.0
        },

        Translate {
            id: translateTransformation
        },

        Scale {
            id: scaleTransformation

            origin.x: animContainer.width / 2.0
            origin.y: animContainer.height / 2.0
        }
    ]

    AdsGrid {
        id: adsGrid
    }

    ParallelAnimation {
        id: cubeRotAnim
        NumberAnimation {
            id: rotAnim
            target: rotationTransformation
            property: "angle"
        }

        NumberAnimation {
            id: transAnim
            target: translateTransformation
            property: "x"
        }

        NumberAnimation {
            id: scaleAnim
            target: scaleTransformation;
            properties: "xScale,yScale";
        }

        onStopped: if(__isExitAnim) {
                       animContainer.visible = false;
                   }
    }
}
