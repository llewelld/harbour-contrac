import QtQuick 2.0
import Sailfish.Silica 1.0
import uk.co.flypig 1.0

Page {
    id: page
    property string token: "abcdef"

    allowedOrientations: Orientation.All

    ExposureConfiguration {
        id: config
    }

    Download {
        id: download
        onDownloadComplete: {
            var keyfiles = [ filename ]

            console.log("Checking: " + filename)

            dbusproxy.provideDiagnosisKeys(keyfiles, config, token)
        }
    }

    DBusProxy {
        id: dbusproxy
    }

    SilicaListView {
        anchors.fill: parent
        VerticalScrollDecorator {}

        PullDownMenu {
            MenuItem {
                //% "About"
                text: qsTrId("contrac-main_about")
                onClicked: pageStack.push(Qt.resolvedUrl("About.qml"))
            }
        }

        header: Column {
            id: column

            width: page.width
            spacing: Theme.paddingLarge
            PageHeader {
                //% "BLE Contact Tracing"
                title: qsTrId("contrac-main_title")
            }

            TextSwitch {
                //% "Scan and send active"
                text: qsTrId("contrac-main_scan")
                width: parent.width - 2 * Theme.horizontalPageMargin
                checked: dbusproxy.isEnabled
                automaticCheck: false
                onClicked: {
                    if (checked) {
                        console.log("Clicked to start")
                        dbusproxy.start();
                    }
                    else {
                        console.log("Clicked to stop")
                        dbusproxy.stop();
                    }
                }
            }

            Label {
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                //% "Sent"
                text: qsTrId("contrac-main_sent") + " : " + dbusproxy.sentCount
                color: Theme.highlightColor
            }

            Label {
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                //% "Received"
                text: qsTrId("contrac-main_received") + " : " + dbusproxy.receivedCount
                color: Theme.highlightColor
            }

            Label {
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                text: "Latest download: " + Qt.formatDate(download.latest, "d MMM yyyy")
                color: Theme.highlightColor
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                enabled: !download.downloading
                text: download.downloading ? "Downloading" : "Download"
                onClicked: download.downloadLatest()
            }
        }
    }
}
