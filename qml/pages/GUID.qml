import QtQuick 2.4
import Sailfish.Silica 1.0
import "../components"
import uk.co.flypig.contrac 1.0

Dialog {
    id: guidPage

    property alias guid: guidInput.text
    property bool validGUID: upload.validateGUID(guid)

    allowedOrientations: Orientation.All
    canAccept: validGUID

    onStatusChanged: {
        console.log("guidPage status: " + status)
        if (status === PageStatus.Active) {
            guidInput.focus = true
        } else if (status === PageStatus.Deactivating) {
            guidInput.focus = false
        }
    }

    onAccepted: {
        console.log("Uploading with (hashed) GUID: " + guid)
        upload.uploadGUID(guid)
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: tokenColumn.height + Theme.paddingLarge
        flickableDirection: Flickable.VerticalFlick

        VerticalScrollDecorator {}

        Column {
            id: tokenColumn
            width: parent.width
            spacing: Theme.paddingLarge

            DialogHeader {
                dialog: guidPage
                //% "GUID entry"
                title: qsTrId("contrac-guid_he_guid_entry")
            }

            Label {
                //% "Please enter the 38-digit GUID that you were given."
                text: qsTrId("contrac-guid_instructions")
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
                id: guidInput
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
                  //Actually, it's A-F hexa sha256, A-Z for Usability, that letters after F appear
                  text = text.replace(/[^0-9A-Z]/g, "")
                }
                maximumLength: 38
                inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhUppercaseOnly
            }

            MouseArea {
                width: parent.width - 2 * Theme.horizontalPageMargin
                height: flow.implicitHeight
                x: Theme.horizontalPageMargin

                onClicked: {
                    guidInput.focus = true
                }

                Flow {
                    id: flow
                    width: parent.width
                    spacing: Theme.paddingSmall
                    //1st Row: 6 digits
                    Row {
                        spacing: Theme.paddingSmall

                        GUIDChar {
                            tan: guid
                            index: 0
                        }
                        GUIDChar {
                            tan: guid
                            index: 1
                        }
                        GUIDChar {
                            tan: guid
                            index: 2
                        }
                        GUIDChar {
                            tan: guid
                            index: 3
                        }
                        GUIDChar {
                            tan: guid
                            index: 4
                        }
                        GUIDChar {
                            tan: guid
                            index: 5
                        }
                        Dash {
                        }
                    }
                    //2nd Row: 8 digits
                    Row {
                        spacing: Theme.paddingSmall

                        GUIDChar {
                            tan: guid
                            index: 6
                        }
                        GUIDChar {
                            tan: guid
                            index: 7
                        }
                        GUIDChar {
                            tan: guid
                            index: 8
                        }
                        GUIDChar {
                            tan: guid
                            index: 9
                        }
                        GUIDChar {
                            tan: guid
                            index: 10
                        }
                        GUIDChar {
                            tan: guid
                            index: 11
                        }
                        GUIDChar {
                            tan: guid
                            index: 12
                        }
                        GUIDChar {
                            tan: guid
                            index: 13
                        }
                        Dash {
                        }
                    }
                    //3rd Row: 4 digits
                    Row {
                        spacing: Theme.paddingSmall

                        GUIDChar {
                            tan: guid
                            index: 14
                        }
                        GUIDChar {
                            tan: guid
                            index: 15
                        }
                        GUIDChar {
                            tan: guid
                            index: 16
                        }
                        GUIDChar {
                            tan: guid
                            index: 17
                        }
                        Dash {
                        }
                    }
                    //4th Row: 4 digits
                    Row {
                        spacing: Theme.paddingSmall

                        GUIDChar {
                            tan: guid
                            index: 18
                        }
                        GUIDChar {
                            tan: guid
                            index: 19
                        }
                        GUIDChar {
                            tan: guid
                            index: 20
                        }
                        GUIDChar {
                            tan: guid
                            index: 21
                        }
                        Dash {
                        }
                    }
                    //5th Row: 4 digits
                    Row {
                        spacing: Theme.paddingSmall

                        GUIDChar {
                            tan: guid
                            index: 22
                        }
                        GUIDChar {
                            tan: guid
                            index: 23
                        }
                        GUIDChar {
                            tan: guid
                            index: 24
                        }
                        GUIDChar {
                            tan: guid
                            index: 25
                        }
                        Dash {
                        }
                    }
                    //6th Row: 12 digits
                    Row {
                        spacing: Theme.paddingSmall

                        GUIDChar {
                            tan: guid
                            index: 26
                        }
                        GUIDChar {
                            tan: guid
                            index: 27
                        }
                        GUIDChar {
                            tan: guid
                            index: 28
                        }
                        GUIDChar {
                            tan: guid
                            index: 29
                        }
                        GUIDChar {
                            tan: guid
                            index: 30
                        }
                        GUIDChar {
                            tan: guid
                            index: 31
                        }
                    }

                    Row {
                        spacing: Theme.paddingSmall

                        GUIDChar {
                            tan: guid
                            index: 32
                        }
                        GUIDChar {
                            tan: guid
                            index: 33
                        }
                        GUIDChar {
                            tan: guid
                            index: 34
                        }
                        GUIDChar {
                            tan: guid
                            index: 35
                        }
                        GUIDChar {
                            tan: guid
                            index: 36
                        }
                        GUIDChar {
                            tan: guid
                            index: 37
                        }
                    }
                }
            }

            Label {
                id: invalidChar
                //% "Invalid entry. Please check your entry."
                text: qsTrId("contrac-guid_invalid-character")
                color: Theme.errorColor
                wrapMode: Text.WordWrap
                anchors {
                    leftMargin: Theme.horizontalPageMargin
                    rightMargin: Theme.horizontalPageMargin
                    left: parent.left
                    right: parent.right
                }

                visible: !upload.validateGUIDCharacters(guid)
            }

            Label {
                //% "Invalid GUID, please check your entry."
                text: qsTrId("contrac-guid_invalid-guid")
                color: Theme.errorColor
                wrapMode: Text.WordWrap
                anchors {
                    leftMargin: Theme.horizontalPageMargin
                    rightMargin: Theme.horizontalPageMargin
                    left: parent.left
                    right: parent.right
                }

                visible: !invalidChar.visible && guid.length === 38 && !validGUID
            }
        }
    }
}
