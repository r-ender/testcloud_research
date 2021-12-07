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

        /*
         // WINDOW PROPERTIES
        visible: true
        width: 600
        height: 680
        color:"steelblue"
    */

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

        BackendStuff {
            id: _afbBackend
        }

            Text{
                id: _speech
                y: 50
                x: 100
                text: "...recording!"
                font.pixelSize: 50
                color: "grey"
            }

            Rectangle{
                    color: "grey"
                    y: 500
                    x: 100
                    width: 500
                    height: _mikropic.implicitHeight
                    border.width: 5
                    border.color: "black"

                    Image {
                        id: _mikropic
                        sourceSize.width: 250
                        sourceSize.height: 200
                        anchors.left: parent.left
                        source: "qrc:/mikro.png"
                    }

                    Text{
                        anchors.left: _mikropic.right
                        anchors.leftMargin: 50
                        text: "press & hold for voice capture"
                        font.pixelSize: 20
                    }

                    MouseArea{
                        anchors.fill: parent
                        onPressAndHold:
                        {
                            _speech.font.pixelSize = 100;
                            _speech.color = "green";
                        }
                    }
            }
        }


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


