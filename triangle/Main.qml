import QtQuick
import QtQuick.Window

Window {
    width: 600
    height: 400
    visible: true
    title: qsTr("Hello Triangle")

    ShaderEffect {
        id: triangle

        anchors.fill: parent
        property color color: "red"

        fragmentShader: "qrc:fragment.frag.qsb"
        vertexShader: "qrc:vertex.vert.qsb"

        smooth: true
    }
}
