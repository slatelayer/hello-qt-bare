
import QtQuick

import "core.mjs" as C


Window {
    id: window

    visibility: Window.FullScreen
    visible: true

    /*
    Timer {
        interval: 1000; running: true; repeat: true
        onTriggered: function() {

            const msg = {
                t: "ping",
                d: Date.now(),
                msg: "PING!"
            }
            core.send(C.encode(msg))
        }
    }
    */

    //Component.onCompleted: function() {}

    Connections {
        target: core
        
        function onRecv (data) {
            const { t, d, m } = C.decode(data)
            
            switch(t){
            case 'heartbeat':
                console.log(`beat ${d}`)
            }
        }
    }
}
