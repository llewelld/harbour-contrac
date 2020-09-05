import QtQuick 2.0
import Sailfish.Silica 1.0
import org.nemomobile.time 1.0
import uk.co.flypig.contrac 1.0

Page {
    id: page
    property string token: "abcdef"
    property alias upload: upload
    property alias download: download
    property ExposureSummary summary;

    allowedOrientations: Orientation.All

    WallClock {
        id: wallclock
        updateFrequency: WallClock.Day
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
            MenuItem {
                //% "Settings"
                text: qsTrId("contrac-main_settings")
                onClicked: pageStack.push(Qt.resolvedUrl("Settings.qml"))
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
                //% "Using the Google/Apple API and a Corona Warn App test server. Uploads/downloads are only for testing."
                text: qsTrId("contrac-main_info")
                color: Theme.highlightColor
                wrapMode: Text.Wrap
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
                //% "Diagnosis keys"
                text: qsTrId("contrac-main_diagnosis_keys")
            }

            Label {
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                //% "Latest upload: "
                text: qsTrId("contrac-main_la_latest-upload") + Qt.formatDate(upload.latest, "d MMM yyyy")
                color: Theme.highlightColor
            }

            Label {
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                //% "Latest download: "
                text: qsTrId("contrac-main_la_latest-download") + Qt.formatDate(download.latest, "d MMM yyyy")
                color: Theme.highlightColor
            }

            Item {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                height: Theme.itemSizeSmall

                BusyIndicator {
                    id: progress
                    anchors.verticalCenter: parent.verticalCenter
                    running: true
                    size: BusyIndicatorSize.Small
                    visible: upload.uploaindg || download.downloading
                }

                Image {
                    anchors.fill: progress
                    visible: !progress.visible
                    source: (upload.status === Upload.StatusError) || (download.status === Download.StatusError) ? "image://theme/icon-s-warning" : "image://theme/icon-s-installed"
                }

                Label {
                    id: statusLabel
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: progress.right
                        right: parent.right
                        leftMargin: Theme.paddingMedium
                    }

                    text: {
                        if (upload.uploading) {
                            //% "Uploading"
                            return qsTrId("contrac-main_la_status-uploading")
                        } else if (download.downloading) {
                            //% "Downloading"
                            return qsTrId("contrac-main_la_status-downloading")
                        } else if (upload.status === Upload.StatusError) {
                            //% "Error uploading"
                            return qsTrId("contrac-main_la_status-upload_error")
                        } else if (download.status === Download.StatusError) {
                            //% "Error downloading"
                            return qsTrId("contrac-main_la_status-download_err0r")
                        } else {
                            //% "No issues"
                            return qsTrId("contrac-main_la_status-no_ssues")
                        }
                    }
                    color: Theme.highlightColor
                }
            }

            Button {
                id: tanButton
                anchors.horizontalCenter: parent.horizontalCenter
                width: Math.max(tanButton.implicitWidth, downloadButton.implicitWidth)
                //% "Enter TeleTAN"
                text: qsTrId("contrac-main_bu_enter-teletan")
                enabled: !upload.uploading
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("TeleTAN.qml"), {upload: page.upload})
                }
            }

            Button {
                id: downloadButton
                anchors.horizontalCenter: parent.horizontalCenter
                width: Math.max(tanButton.implicitWidth, downloadButton.implicitWidth)
                enabled: !download.downloading
                //% "Download latest keys"
                text: qsTrId("contrac-main_bu_download-keys")
                onClicked: {
                    download.downloadLatest()
                    pageStack.push(Qt.resolvedUrl("DownloadInfo.qml"), {download: page.download})
                }
            }

            Button {
                id: performCheckButton
                anchors.horizontalCenter: parent.horizontalCenter
                width: Math.max(tanButton.implicitWidth, downloadButton.implicitWidth)
                enabled: !download.downloading
                text: "Perform check"
                onClicked: {
                    var filelist = download.fileList();
                    console.log("Files to check: " + filelist.length);
                    dbusproxy.provideDiagnosisKeys(filelist, download.config, token);
                    summary = dbusproxy.getExposureSummary(token);
                    console.log("Attenuation durations: " + summary.attenuationDurations)
                    console.log("Days since last exposure: " + summary.daysSinceLastExposure)
                    console.log("Matched key count: " + summary.matchedKeyCount)
                    console.log("Maximum risk score: " + summary.maximumRiskScore)
                    console.log("Summation risk score: " + summary.summationRiskScore)
                }
            }
        }
    }
}
