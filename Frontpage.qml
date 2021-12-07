import QtQuick 2.6
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0
import QtQuick.Window 2.0
import io.qt.BackendStuff 1.0

Page {
        id: _front
        header: Label{
            color: "steelblue"
            padding: 10
            text: "This is MoTalk-App"
            font.pixelSize: 20
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

            Image {
                    id: _backgroundFreq
                    width: parent.width
                    height: parent.height
                    source: "background_frequency.jpg"
                    fillMode: Image.Tile; opacity: 0.3
            }


            ListView{
                    id: _listView
                    anchors.fill: parent
                    topMargin: 80
                    leftMargin: 60
                    bottomMargin: 80
                    rightMargin: 60
                    spacing: 40

                    //model: model
                    Rectangle{
                            color: "grey"
                            y: 100
                            x: 50
                            width: _listView.width - _listView.leftMargin - _listView.rightMargin
                            height: _avatar_Hiller.implicitHeight
                            border.width: 5
                            border.color: "black"


                            Image {
                                id: _avatar_Hiller
                                sourceSize.width: 250
                                sourceSize.height: 160
                                anchors.left: parent.left
                                source: "qrc:/Marco_Hiller.jpeg"
                            }

                            Text{
                                anchors.left: _avatar_Hiller.right
                                y: 70
                                anchors.leftMargin: 20
                                text: "Choose type-option"
                                font.pixelSize: 20
                            }

                            MouseArea{
                                anchors.fill: parent
                                onClicked: _front.StackView.view.push("qrc:/type_option.qml")
                            }
                    }

                    Rectangle{
                            color: "grey"
                            y: 400
                            x: 50
                            width: _listView.width - _listView.leftMargin - _listView.rightMargin
                            height: _avatar_Lex.implicitHeight
                            //leftPadding: _avatar.implicitWidth + 32
                            border.width: 5
                            border.color: "black"

                            Image {
                                id: _avatar_Lex
                                anchors.left: parent.left
                                source: "qrc:/Stefan_Lex.jpeg"
                                sourceSize.width: 130
                            }

                            Text{
                                anchors.left: _avatar_Lex.right
                                y: 60
                                anchors.leftMargin: 20
                                text: "Choose speech-option"
                                font.pixelSize: 20
                            }

                            MouseArea{
                                anchors.fill: parent
                                onClicked: _front.StackView.view.push("qrc:/speech_option.qml")
                            }
                    }
                }
    }
