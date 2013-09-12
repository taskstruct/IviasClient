import QtQuick 2.1
import QtGraphicalEffects 1.0

Item {
    id: mirrorEffect

    property alias mirroredItem: effectSource.sourceItem

    // makes horizontal blur. Its output is used by second shader effect.
    ShaderEffect {
        id: bufferShader

        anchors.fill: parent

        // Properties which will be passed into the shader as uniforms
        property real xStep: 1/width

        property variant source: ShaderEffectSource {
            id: effectSource
            smooth: true
            sourceRect: Qt.rect( 0, sourceItem.height- mirrorEffect.height, mirrorEffect.width, mirrorEffect.height )
            wrapMode: ShaderEffectSource.ClampToEdge
        }

        vertexShader: "
            uniform highp mat4 qt_Matrix;
            attribute highp vec4 qt_Vertex;
            attribute highp vec2 qt_MultiTexCoord0;
            varying highp vec2 qt_TexCoord0;
            uniform highp float height;

            void main() {
                mat4 scale = mat4(
                        vec4(    1.0,  0.0, 0.0, 0.0),
                        vec4(    0.0, -1.0, 0.0, 0.0),
                        vec4(    0.0,  0.0, 1.0, 0.0),
                        vec4(    0.0,  0.0, 0.0, 1.0)
                    );

                vec4 pos = scale * qt_Vertex;

                pos.y += height;

                qt_TexCoord0 = qt_MultiTexCoord0;
                gl_Position = qt_Matrix * pos;
            }"

        fragmentShader: "
            uniform sampler2D source;
            varying highp vec2 qt_TexCoord0;
            uniform highp float xStep;

            void main(void)
            {
                vec4 sum = vec4(0.0);

                // blur in x (horizontal)
                // take nine samples, with the distance xStep between them
//                sum += texture2D(source, vec2(qt_TexCoord0.x - 4.0 * xStep, qt_TexCoord0.y)) * 0.05;
//                sum += texture2D(source, vec2(qt_TexCoord0.x - 3.0 * xStep, qt_TexCoord0.y)) * 0.09;
//                sum += texture2D(source, vec2(qt_TexCoord0.x - 2.0 * xStep, qt_TexCoord0.y)) * 0.12;
//                sum += texture2D(source, vec2(qt_TexCoord0.x - xStep,       qt_TexCoord0.y)) * 0.15;
//                sum += texture2D(source, vec2(qt_TexCoord0.x,               qt_TexCoord0.y)) * 0.16;
//                sum += texture2D(source, vec2(qt_TexCoord0.x + xStep,       qt_TexCoord0.y)) * 0.15;
//                sum += texture2D(source, vec2(qt_TexCoord0.x + 2.0 * xStep, qt_TexCoord0.y)) * 0.12;
//                sum += texture2D(source, vec2(qt_TexCoord0.x + 3.0 * xStep, qt_TexCoord0.y)) * 0.09;
//                sum += texture2D(source, vec2(qt_TexCoord0.x + 4.0 * xStep, qt_TexCoord0.y)) * 0.05;

                sum += texture2D(source, vec2(qt_TexCoord0.x - xStep,       qt_TexCoord0.y)) * 0.25;
                sum += texture2D(source, vec2(qt_TexCoord0.x,               qt_TexCoord0.y)) * 0.5;
                sum += texture2D(source, vec2(qt_TexCoord0.x + xStep,       qt_TexCoord0.y)) * 0.25;

                gl_FragColor = sum;
            }"
    }

    ShaderEffect {
        id: finalEffect

        anchors.fill: parent

        mesh: GridMesh { resolution: Qt.size(20,20) }

        // Properties which will be passed into the shader as uniforms
        property real yStep: 1/height

        property variant source: ShaderEffectSource {
            hideSource: true
            smooth: true
            sourceItem: bufferShader
            wrapMode: ShaderEffectSource.ClampToEdge
        }

        vertexShader: "
            attribute highp vec4 qt_Vertex;
            attribute highp vec2 qt_MultiTexCoord0;
            uniform highp mat4 qt_Matrix;
            varying highp vec2 qt_TexCoord0;
            uniform highp float width;

            void main(void)
            {
                highp vec4 pos = qt_Vertex;

                highp float d = -0.01 * qt_MultiTexCoord0.y; // how wide to be

                pos.x = width * mix(d, 1.0 - d, qt_MultiTexCoord0.x);

                gl_Position = qt_Matrix * pos;

                qt_TexCoord0 = qt_MultiTexCoord0;
            }"

        fragmentShader: "
            uniform sampler2D source;
            varying highp vec2 qt_TexCoord0;
            uniform highp float yStep;

            void main(void)
            {
                vec4 sum = vec4(0.0);

                // blur in y (vertical)
                // take nine samples, with the distance yStep between them
//                sum += texture2D(source, vec2(qt_TexCoord0.x, qt_TexCoord0.y - 4.0 * yStep)) * 0.05;
//                sum += texture2D(source, vec2(qt_TexCoord0.x, qt_TexCoord0.y - 3.0 * yStep)) * 0.09;
//                sum += texture2D(source, vec2(qt_TexCoord0.x, qt_TexCoord0.y - 2.0 * yStep)) * 0.12;
//                sum += texture2D(source, vec2(qt_TexCoord0.x, qt_TexCoord0.y - yStep      )) * 0.15;
//                sum += texture2D(source, vec2(qt_TexCoord0.x, qt_TexCoord0.y              )) * 0.16;
//                sum += texture2D(source, vec2(qt_TexCoord0.x, qt_TexCoord0.y + yStep      )) * 0.15;
//                sum += texture2D(source, vec2(qt_TexCoord0.x, qt_TexCoord0.y + 2.0 * yStep)) * 0.12;
//                sum += texture2D(source, vec2(qt_TexCoord0.x, qt_TexCoord0.y + 3.0 * yStep)) * 0.09;
//                sum += texture2D(source, vec2(qt_TexCoord0.x, qt_TexCoord0.y + 4.0 * yStep)) * 0.05;

                sum += texture2D(source, vec2(qt_TexCoord0.x, qt_TexCoord0.y - yStep      )) * 0.25;
                sum += texture2D(source, vec2(qt_TexCoord0.x, qt_TexCoord0.y              )) * 0.5;
                sum += texture2D(source, vec2(qt_TexCoord0.x, qt_TexCoord0.y + yStep      )) * 0.25;

                gl_FragColor = sum * (1.0 - qt_TexCoord0.y);
            }
        "

    }
}
