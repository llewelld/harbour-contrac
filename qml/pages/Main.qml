import QtQuick 2.0
import Sailfish.Silica 1.0
import org.nemomobile.time 1.0
import uk.co.flypig 1.0

Page {
    id: page
    property string token: "abcdef"

    allowedOrientations: Orientation.All

    WallClock {
        id: wallclock
        updateFrequency: WallClock.Day
    }

    ExposureConfiguration {
        id: config
    }

    function moreThanADayAgo(latest) {
        var result = true
        if (!isNaN(latest)) {
            var today = wallclock.time
            today.setSeconds(0)
            today.setMinutes(0)
            today.setHours(0)
            today.setMilliseconds(0)
            result = ((today - latest) > (24 * 60 * 60 * 1000))
        }
        return result
    }

    Download {
        id: download
        property bool available: moreThanADayAgo(latest)
        onDownloadComplete: {
            var keyfiles = [ filename ]

            console.log("Checking: " + filename)

            dbusproxy.provideDiagnosisKeys(keyfiles, config, token)
        }
    }

    Upload {
        id: upload
        property bool available: moreThanADayAgo(latest)
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

            Label {
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                //% "Using the Google/Apple API"
                text: qsTrId("contrac-main_using_google_apple_api")
                color: Theme.highlightColor
            }

            Label {
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                //% "Using the Corona Warn App servers"
                text: qsTrId("contrac-main_using_corona_warn_app_servers")
                color: Theme.highlightColor
            }

            SectionHeader {
                //% "Contact beacons"
                text: qsTrId("contrac-main_contact_beacons")
            }

            TextSwitch {
                //% "Scan and send active"
                text: qsTrId("contrac-main_scan")
                width: parent.width - 2 * Theme.horizontalPageMargin
                checked: dbusproxy.isEnabled
                busy: dbusproxy.isBusy
                automaticCheck: false
                onClicked: {
                    if (!checked) {
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

            SectionHeader {
                //% "Diagnosis key uploads"
                text: qsTrId("contrac-main_diagnosis_key_uploadss")
            }

            Label {
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                text: "Latest upload: " + Qt.formatDate(upload.latest, "d MMM yyyy")
                color: Theme.highlightColor
            }

            ProgressBar {
                id: uploadProgress
                width: parent.width
                maximumValue: 100
                value: upload.available ? upload.progress * 100.0 : 100.0
                label: (upload.status == Upload.StatusError) ? "Error" : "Upload progress"
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                enabled: !upload.uploading && (dbusproxy.sentCount > 0) && upload.available
                text: upload.uploading ? "Uploading" : "Upload"
                onClicked: {
                    console.log("Uploading")
                    upload.upload("R3ZNUEV9JA")
                }
            }

            SectionHeader {
                //% "Diagnosis key downloads"
                text: qsTrId("contrac-main_diagnosis_key_downloads")
            }

            Label {
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                text: "Latest download: " + Qt.formatDate(download.latest, "d MMM yyyy")
                color: Theme.highlightColor
            }

            ProgressBar {
                id: downloadProgress
                width: parent.width
                maximumValue: 100
                value: download.available ? download.progress * 100.0 : 100.0
                label: "Download progress"
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                enabled: !download.downloading && download.available
                text: download.downloading ? "Downloading" : "Download"
                onClicked: download.downloadLatest()
            }
        }
    }
}
