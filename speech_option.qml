/* import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
*/
import QtQuick 2.6
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0
import QtQuick.Window 2.0
import io.qt.BackendStuff 1.0


Page {
        id: _root
        signal triggert
        property bool voice_cap_time: false

        property int defaultSeconds: 2
        property int seconds: defaultSeconds


        /*
         // WINDOW PROPERTIES
        visible: true
        width: 600
        height: 680
        color:"steelblue"
    */
        BackendStuff{
            id: _afbBackend
        }

        Button{
            id: _backButton
            x:0
            y:0
            text: " <-- Back"
            enabled: true
            onClicked: _stack.pop()
        }

        //child-object will be displayed inside root-window
        Text {
                id: _MoTalk
                text: "This is Motalk-App"
                x: 100
                y: 10
                //anchors.horizontalCenter: _root.horizontalCenter
                font.pointSize: 24; font.bold: true
        }

        Image {
                id: _backgroundFreq
                anchors.fill: parent
                width: parent.width
                height: parent.height
                source: "background_frequency.jpg"
                fillMode: Image.Tile; opacity: 0.3
        }

            Text{
                id: _speech
                y: 250
                x: 75
                text: "...recording for 5 sec!"
                font.pixelSize: 40
                color: "grey"
                }

            Text{
                id: _speech2
                y: 500
                x: 75
                text: "...listening to voice-message!"
                font.pixelSize: 40
                color: "grey"
                }

           Timer {
                   id: innerTimer
                   //repeat: true
                   //interval: 1000
                   onTriggered: {
                       running = true;
                       //_speech.color = "green";
                       //_speech.font.pixelSize = 80;
                       _afbBackend.capture_voice();
                       //running = true;
                       /*
                       _root.seconds--;
                       if (_root.seconds == 0) {
                           running = false;
                           _root.seconds = _root.defaultSeconds
                           _speech.color = "grey"
                           _speech.font.pixelSize = 40;
                           _root.triggert()
                       }*/
                       _speech.color = "grey"
                       _speech.font.pixelSize = 40;
                       running = false;
                   }
               }

           Timer {
                   id: innerTimer2
                   //repeat: true
                   //interval: 1000
                   onTriggered: {
                       running = true;
                       _afbBackend.listen_msg();
                       _speech2.color = "grey"
                       _speech2.font.pixelSize = 40;
                       running = false;
                   }
               }

            Rectangle{
                    id: _rect3
                    color: "grey"
                    y: 150
                    x: 100
                    width: 460
                    height: _mikropic.implicitHeight + 20
                    border.width: 5
                    border.color: "black"

                    Image {
                        id: _mikropic
                        anchors.left: _rect3.left
                        anchors.leftMargin: 10
                        anchors.top: _rect3.top
                        anchors.topMargin: 10
                        sourceSize.width: 250
                        sourceSize.height: 200
                        source: "qrc:/mikro.png"
                    }

                    Text{
                        anchors.left: _mikropic.right
                        anchors.leftMargin: 10
                        anchors.rightMargin: 10
                        anchors.top: _rect3.top
                        anchors.topMargin: 35
                        text: "press and wait 1sec for recording"
                        font.pixelSize: 23
                    }

                    /*
                    Timer{
                        id: _timer2
                        interval: 1000 //fühlt sich wie 5se an
                        repeat: false
                        running: false

                        onTriggered:
                        {
                            _speech.font.pixelSize = 80;
                            _speech.color = "green";
                        }
                    } */

                    MouseArea{
                        id: _mouseArea1
                        anchors.fill: parent

                        onClicked:
                        {
                            _speech.font.pixelSize = 80;
                            _speech.color = "green";
                            innerTimer.start();
                        }
                    }

            }


            Rectangle{
                    id: _rect4
                    color: "grey"
                    y: 400
                    x: 100
                    width: 460
                    height: _speakerpic.implicitHeight + 80
                    border.width: 5
                    border.color: "black"

                    Image {
                        id: _speakerpic
                        anchors.left: _rect4.left
                        anchors.leftMargin: 40
                        anchors.top: _rect4.top
                        anchors.topMargin: 40
                        sourceSize.width: 250
                        sourceSize.height: 200
                        source: "qrc:/lautsprecher.png"
                    }

                    Text{
                        anchors.left: _speakerpic.right
                        anchors.leftMargin: 50
                        anchors.rightMargin: 10
                        anchors.top: _rect4.top
                        anchors.topMargin: 35
                        text: "listen to voice-message"
                        font.pixelSize: 23
                    }

                    MouseArea{
                        id: _mouseArea2
                        anchors.fill: parent

                        onClicked:
                        {
                            _speech2.font.pixelSize = 80;
                            _speech2.color = "green";
                            innerTimer2.start();
                        }
                    }

            }
 }


/*
            Rectangle{
                    id: _rect4
                    color: "grey"
                    y: 300
                    x: 100
                    width: 450
                    height: _mikropic.implicitHeight + 20
                    border.width: 5
                    border.color: "black"

                    signal xpressed()

                    Timer{
                        id: _timer1
                        interval: 5000 //press-and-hold
                        repeat: false
                        running: false

                        onTriggered: {
                            _rect4.xpressed();
                        }
                    }

                    Image {
                        id: _mikropic4
                        anchors.left: _rect4.left
                        anchors.leftMargin: 10
                        anchors.top: _rect4.top
                        anchors.topMargin: 10
                        sourceSize.width: 250
                        sourceSize.height: 200
                        source: "qrc:/mikro.png"
                    }

                    Text{
                        anchors.left: _mikropic4.right
                        anchors.leftMargin: 25
                        anchors.top: _rect4.top
                        anchors.topMargin: 35
                        text: "press for 5sec voice capturing"
                        font.pixelSize: 23
                    }

                    //MouseArea{
                      //  anchors.fill: parent

                        onPressedChanged:
                        {
                            if(pressed) {
                                _timer1.running = true;

                                _speech.font.pixelSize = 80;
                                _speech.color = "green";
                            }

                            else
                            {
                                _timer1.running = false;
                                _speech.font.pixelSize = 40;
                                _speech.color = "grey";
                            }
                    }
            //}
}

}

*/


                /*
                    Rectangle {  //Pane is a rectangle whose color comes from the application's style, similar to Frame, without stroke around its border.
                        id: _rect2
                        Layout.fillWidth: true
                        x: 50
                        y: 550
                        RowLayout {
                            width: parent.width
                            Button {
                                id: _sendButton
                                text: "Send"
                                //enabled: _textfield.length > 0   //only enable when message to send
                                enabled: true
                                onClicked:
                                {
                                    //_afbBackend.sendMessage("motalk2021","ping", true);
                                    _afbBackend.sendMessage("motalk2021","rewr",{"index":"blub"});
                                    //_afbBackend.sendMessage("motalk2021","rewr",true);
                                }
                            }

                            Button {
                                id: _connectButton
                                text: "connect AFB-ws"
                                enabled: true
                                onClicked:
                                {
                                    _afbBackend.connectClicked();
                                    _afbBackend.sendClicked("GET /api?token=123456 HTTP/1.1\r\nHost: 127.0.0.1:1234\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Version: 13\r\nSec-WebSocket-Key: MfpIVG082jFTV7SxTNNijQ==\r\nSec-WebSocket-Protocol: x-afb-ws-json1\r\nContent-Length: 0\r\n\r\n");
                                }
                            }

                            Button {
                                id: _disconnectButton
                                text: "disconnect AFB-ws"
                                enabled: true
                                onClicked:
                                {
                                    _afbBackend.disconnectClicked();
                                }

                            }

                            Button {
                                id: _UserInput
                                text: "ParseUserInput"
                                enabled: true
                                onClicked:
                                {
                                    _afbBackend.UserInput = _textfield.text;
                                    _afbBackend.sendMessage("motalk2021","rewr");
                                    //_afbBackend.sendKeyboardInput("testbutton");
                                    //_afbBackend.receiveKeypress = _textfield.selectAll();
                                }
                            }
                        }

                } */


