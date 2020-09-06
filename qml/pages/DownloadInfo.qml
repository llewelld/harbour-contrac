import QtQuick 2.4
import Sailfish.Silica 1.0
import "../components"
import uk.co.flypig.contrac 1.0

Page {
    id: downloadPage

    allowedOrientations: Orientation.All
    property Download download

    function errorString() {
        switch (download.error) {
        case Download.ErrorNetwork:
            //% "Network error"
            return qsTrId("contrac-download_error_network-error")
        }
        //% "No error"
        return qsTrId("contrac-download_error_none")
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: downloadColumn.height + Theme.paddingLarge
        flickableDirection: Flickable.VerticalFlick

        VerticalScrollDecorator {}

        Column {
            id: downloadColumn
            width: parent.width
            spacing: Theme.paddingLarge

            PageHeader {
                //% "Downloading"
                title: qsTrId("contrac-download_he_downloading")
            }

            Label {
                //% "Please wait while keys are downloaded."
                text: qsTrId("contrac-download_instructions")
                wrapMode: Text.WordWrap
                font.pixelSize: Theme.fontSizeMedium
                anchors {
                    leftMargin: Theme.horizontalPageMargin
                    rightMargin: Theme.horizontalPageMargin
                    left: parent.left
                    right: parent.right
                }
                color: Theme.highlightColor
            }

            ProgressCircle {
                id: downloadProgress

                anchors.horizontalCenter: parent.horizontalCenter
                width: Theme.itemSizeLarge
                height: width
                value: 0

                Timer {
                    interval: 35
                    repeat: true
                    onTriggered: {
                        var max = download.downloading ? download.progress : 1.0
                        if (downloadProgress.value < max) {
                            downloadProgress.value += 0.01
                        }
                        if (download.status === Download.StatusError) {
                            downloadProgress.value = 1.0
                        }
                    }
                    running: downloadPage.status === PageStatus.Active
                }
            }

            Label {
                text: {
                    if (downloadProgress.value >= 1.0) {
                        if (download.status === Download.StatusError) {
                            return errorString()
                        } else if (download.status === Download.StatusIdle) {
                            //% "Completed"
                            return qsTrId("contrac-download_la_completed")
                        }
                    }
                    return Math.round(downloadProgress.value * 100.0) + "%"
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
                color: Theme.highlightColor
            }
        }
    }

    Timer {
        interval: 5 * 1000
        repeat: false
        onTriggered: pageStack.pop()
        running: downloadProgress.value >= 1.0
    }
}
