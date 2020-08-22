import QtQuick 2.4
import Sailfish.Silica 1.0
import "../components"
import uk.co.flypig.contrac 1.0

Page {
    id: uploadPage

    allowedOrientations: Orientation.All
    property Upload upload

    function errorString() {
        switch (upload.error) {
        case Upload.ErrorNone:
            //% "No error"
            return qsTrId("contrac-upload_error_none")
        case Upload.ErrorInvalidTAN:
            //% "Error: Invalid TAN"
            return qsTrId("contrac-upload_error_invalid-tan")
        case Upload.ErrorInvalidRegToken:
            //% "Error: Invalid registration token"
            return qsTrId("contrac-upload_error_invalid-reg-token")
        case Upload.ErrorInvalidDiagnosisKeys:
            //% "Error: Invalid diagnosis keys"
            return qsTrId("contrac-upload_error_invalid-diagnosis-keys")
        case Upload.ErrorNoDiagnosisKeys:
            //% "Error: No diagnosis keys to upload"
            return qsTrId("contrac-upload_error_no-diagnosis-keys")
        case Upload.ErrorNetwork:
            //% "Network error"
            return qsTrId("contrac-upload_error_network-error")
        case Upload.ErrorParsing:
            //% "Error parsing reply from server"
            return qsTrId("contrac-upload_error_parsing")
        case Upload.ErrorInternal:
            //% "Internal state error"
            return qsTrId("contrac-upload_error_internal-state")
        }
        //% "Unkown error"
        return qsTrId("contrac-upload_error_unknown")
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: uploadColumn.height + Theme.paddingLarge
        flickableDirection: Flickable.VerticalFlick

        VerticalScrollDecorator {}

        Column {
            id: uploadColumn
            width: parent.width
            spacing: Theme.paddingLarge

            PageHeader {
                //% "Uploading"
                title: qsTrId("contrac-upload_he_uploading")
            }

            Label {
                //% "Please wait while your keys are uploaded."
                text: qsTrId("contrac-upload_instructions")
                wrapMode: Text.WordWrap
                font.pixelSize: Theme.fontSizeMedium
                anchors {
                    leftMargin: Theme.horizontalPageMargin
                    rightMargin: Theme.horizontalPageMargin
                    left: parent.left
                    right: parent.right
                }
            }

            ProgressCircle {
                id: uploadProgress

                anchors.horizontalCenter: parent.horizontalCenter
                width: Theme.itemSizeLarge
                height: width
                value: 0

                Timer {
                    interval: 35
                    repeat: true
                    onTriggered: {
                        var max = upload.uploading ? upload.progress : 1.0
                        if (uploadProgress.value < max) {
                            uploadProgress.value += 0.05
                        }
                        if (upload.status === Upload.StatusError) {
                            uploadProgress.value = 1.0
                        }
                    }
                    running: uploadPage.status === PageStatus.Active
                }
            }

            Label {
                text: {
                    if (uploadProgress.value >= 1.0) {
                        if (upload.status === Upload.StatusError) {
                            return errorString()
                        } else if (upload.status === Upload.StatusIdle) {
                            return "Completed"
                        }
                    }
                    return Math.round(uploadProgress.value * 100.0) + "%"
                }
                horizontalAlignment: Text.Center
                wrapMode: Text.WordWrap
                font.pixelSize: Theme.fontSizeMedium
                anchors {
                    leftMargin: Theme.horizontalPageMargin
                    rightMargin: Theme.horizontalPageMargin
                    left: parent.left
                    right: parent.right
                }
            }
        }
    }

    Timer {
        interval: 5 * 1000
        repeat: false
        onTriggered: pageStack.pop()
        running: upload.status === Upload.StatusIdle
    }
}
