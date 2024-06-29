
import QtQuick
import QtQuick.Controls

import "core.mjs" as C


Window {
    id: window
    color: "#050505"
    visibility: Window.FullScreen
    visible: true

    Label {
        id: title
        text: ""
        font.pointSize: 48
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }

    Button {
        id: clicker
        text: "Click"
        visible: false
        onClicked: _ => {
            const msg = {
                typ: "click"
            }
            core.send(C.encode(msg))
        }
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: title.bottom
        anchors.topMargin: 20
    }

    Label {
        id: beat
        color: "#666666"
        font.pointSize: 24
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }

    Connections {
        target: core
        
        function onRecv (data) {
            const { typ, ts, msg } = C.decode(data)
            
            switch(typ){
            case 'color':
                window.color = msg.color
                return

            case 'title':
                title.text = msg.title
                title.color = msg.color
                return

            case 'showButton':
                clicker.visible = true
                return

            case 'heartbeat':
                beat.text = ts
                return
            }
        }
    }
}
