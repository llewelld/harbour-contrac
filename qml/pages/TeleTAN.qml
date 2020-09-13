    import QtQuick 2.4
import Sailfish.Silica 1.0
import "../components"
import uk.co.flypig.contrac 1.0

Dialog {
    id: teletanPage

    property alias teletan: teletanInput.text
    property bool validTan: upload.validateTeleTAN(teletan)

    allowedOrientations: Orientation.All
    canAccept: validTan

    onStatusChanged: {
        console.log("teletanPage status: " + status)
        if (status === PageStatus.Active) {
            teletanInput.focus = true
        } else if (status === PageStatus.Deactivating) {
            teletanInput.focus = false
        }
    }

    onAccepted: {
        console.log("Uploading with tan: " + teletan)
        upload.upload(teletan)
        pageStack.replace(Qt.resolvedUrl("UploadInfo.qml"))
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: teletanColumn.height + Theme.paddingLarge
        flickableDirection: Flickable.VerticalFlick

        VerticalScrollDecorator {}

        Column {
            id: teletanColumn
            width: parent.width
            spacing: Theme.paddingLarge

            DialogHeader {
                dialog: teletanPage
                //% "TAN entry"
                title: qsTrId("contrac-teletan_he_tan_entry")
            }

            Label {
                //% "Please enter the 10-digit TAN that you were given."
                text: qsTrId("contrac-teletan_instructions")
                wrapMode: Text.WordWrap
                font.pixelSize: Theme.fontSizeMedium
                color: Theme.highlightColor
                anchors {
                    leftMargin: Theme.horizontalPageMargin
                    rightMargin: Theme.horizontalPageMargin
                    left: parent.left
                    right: parent.right
                }
            }

            TextField {
                id: teletanInput
                //text: "R3ZNUEV9JA"
                visible: false
                focus: true
                anchors {
                    leftMargin: Theme.horizontalPageMargin
                    rightMargin: Theme.horizontalPageMargin
                    left: parent.left
                    right: parent.right
                }
                onTextChanged: {
                  text = text.toUpperCase()
                  text = text.replace(/[^0-9A-Z]/g, "")
                }
                maximumLength: 10
                inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
            }

            MouseArea {
                width: parent.width - 2 * Theme.horizontalPageMargin
                height: flow.implicitHeight
                x: Theme.horizontalPageMargin

                onClicked: {
                    teletanInput.focus = true
                }


                Flow {
                    id: flow
                    width: parent.width
                    spacing: Theme.paddingSmall

                    Row {
                        spacing: Theme.paddingSmall

                        TanChar {
                            tan: teletan
                            index: 0
                        }
                        TanChar {
                            tan: teletan
                            index: 1
                        }
                        TanChar {
                            tan: teletan
                            index: 2
                        }
                        Dash {
                        }
                    }

                    Row {
                        spacing: Theme.paddingSmall

                        TanChar {
                            tan: teletan
                            index: 3
                        }
                        TanChar {
                            tan: teletan
                            index: 4
                        }
                        TanChar {
                            tan: teletan
                            index: 5
                        }
                        Dash {
                        }
                    }

                    Row {
                        spacing: Theme.paddingSmall

                        TanChar {
                            tan: teletan
                            index: 6
                        }
                        TanChar {
                            tan: teletan
                            index: 7
                        }
                        TanChar {
                            tan: teletan
                            index: 8
                        }
                        TanChar {
                            tan: teletan
                            index: 9
                        }
                    }
                }
            }

            Label {
                id: invalidChar
                //% "Invald entry. Please check your entry."
                text: qsTrId("contrac-teletan_invalid-character")
                color: Theme.errorColor
                wrapMode: Text.WordWrap
                anchors {
                    leftMargin: Theme.horizontalPageMargin
                    rightMargin: Theme.horizontalPageMargin
                    left: parent.left
                    right: parent.right
                }

                visible: !upload.validateTeleTANCharacters(teletan)
            }

            Label {
                //% "Invald TAN, please check your entry."
                text: qsTrId("contrac-teletan_invalid-tan")
                color: Theme.errorColor
                wrapMode: Text.WordWrap
                anchors {
                    leftMargin: Theme.horizontalPageMargin
                    rightMargin: Theme.horizontalPageMargin
                    left: parent.left
                    right: parent.right
                }

                visible: !invalidChar.visible && teletan.length === 10 && !validTan
            }

            Label {
                //% "The only TANs the test server accepts are R3ZNUEV9JA and CG4Z5A9CY9."
                text: qsTrId("contrac-teletan_describe-valid-tans")
                color: Theme.highlightColor
                wrapMode: Text.WordWrap
                anchors {
                    leftMargin: Theme.horizontalPageMargin
                    rightMargin: Theme.horizontalPageMargin
                    left: parent.left
                    right: parent.right
                }
            }
        }
    }
}
