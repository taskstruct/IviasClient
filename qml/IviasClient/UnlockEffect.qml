import QtQuick 2.1
import QtGraphicalEffects 1.0



Item {
    id: unlockEffect
    width: 250
    height: 250

    Image {
        id: grid
        source: "images/dots_grid.png"
        anchors.fill: parent
    }


    ShaderEffectSource {
        id: effectSource
        hideSource: true
        smooth: true
        visible: false
        sourceItem: grid
    }

    ShaderEffect {
        anchors.fill: parent

        property real wave: 0.3
        property real waveOriginX: 0.5
        property real waveOriginY: 0.5
        property real waveWidth: 0.2
        property real aspectRatio: width/height
        property variant source: effectSource

        fragmentShader: "
        varying highp vec2 qt_TexCoord0;
        uniform lowp float qt_Opacity;
        uniform sampler2D source;
        uniform highp float wave;
        uniform highp float waveWidth;
        uniform highp float waveOriginX;
        uniform highp float waveOriginY;
        uniform highp float aspectRatio;

        void main(void)
        {
            mediump vec2 origin = vec2(waveOriginX, (1.0 - waveOriginY) / aspectRatio);
            highp float fragmentDistance = distance(vec2(qt_TexCoord0.s, qt_TexCoord0.t / aspectRatio), origin);
            highp float waveLength = waveWidth + fragmentDistance * 0.25;
            highp float finalOpacity = 0.0;

            if ( fragmentDistance > wave && fragmentDistance < wave + waveLength) {
                highp float distanceFromWaveEdge = min(abs(wave - fragmentDistance), abs(wave + waveLength - fragmentDistance));
                finalOpacity = distanceFromWaveEdge / ( waveWidth / 2.0 );
            }

            highp vec4 sourceColor = texture2D(source, qt_TexCoord0.st);
            gl_FragColor = vec4( sourceColor.rgba*finalOpacity) * qt_Opacity;
        }"

        SequentialAnimation on wave {
            id: waveAnim

            loops: Animation.Infinite
            running: true

            NumberAnimation {
                easing.type: Easing.Linear
                from: 0.0; to: 1.0;
                duration: 2000
            }

            PauseAnimation { duration: 1000 }
        }
    }
}


