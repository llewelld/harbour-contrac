import QtQuick 2.4
import Sailfish.Silica 1.0
import QtMultimedia 5.6
import Amber.QrFilter 1.0
import "../components"
import uk.co.flypig.contrac 1.0

Dialog {
    id: qrPage

    property alias guid: qrFilter.result
    property bool validGUID: upload.validateGUID(guid)

    allowedOrientations: Orientation.All
    canAccept: validGUID

    onStatusChanged: {
        console.log("QRScanPage status: " + status)
    }

    onAccepted: {
        console.log("Uploading with (hashed) GUID: " + guid)
        upload.uploadGUID(guid)
    }

    Camera {
          id: camera
          flash.mode: Camera.FlashOff
          focus.focusMode: Camera.FocusContinuous
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: tokenColumn.height + Theme.paddingLarge
        flickableDirection: Flickable.VerticalFlick

        Column {
            id: tokenColumn
            width: parent.width
            spacing: Theme.paddingLarge

            DialogHeader {
                dialog: qrPage
                //% "Scan QR-Code"
                title: qsTrId("contrac-qrscan_scan_qrcode")
            }

            Label {
                //% "Invalid QR-Code."
                text: qsTrId("contrac-guid_invalid-guid")
                color: Theme.errorColor
                wrapMode: Text.WordWrap
                anchors {
                    leftMargin: Theme.horizontalPageMargin
                    rightMargin: Theme.horizontalPageMargin
                    left: parent.left
                    right: parent.right
                }

                visible: guid && !validGUID
            }

            VideoOutput {
                source:  camera
                filters: [ qrFilter ]
            }

            QrFilter {
                id: qrFilter
            }
        }
    }
}
