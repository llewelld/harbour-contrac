import QtQuick 2.0
import Sailfish.Silica 1.0
import uk.co.flypig.contrac 1.0

Page {
    id: page
    property string token: "abcdef"
    property bool updatePending

    allowedOrientations: Orientation.All

    function translateRiskScore(risk) {
        if (risk < 15) {
            return "Low"
        } else {
            return "Ligh"
        }
    }

    function performUpdate() {
        updatePending = false
        var filelist = download.fileList()
        console.log("Files to check: " + filelist.length)
        updating = true
        dbusproxy.provideDiagnosisKeys(filelist, download.config, token)
    }

    Connections {
        target: download

        onAllFilesDownloaded: {
            // The download just finished
            if (page.status === PageStatus.Active) {
                performUpdate()
            }
            else {
                updatePending = true
            }
        }
    }

    onStatusChanged: {
        if (status === PageStatus.Active) {
            if (Settings.infoViewed === 0) {
                pageStack.push(Qt.resolvedUrl("Info.qml"));
                Settings.infoViewed = 1;
            }
            if (updatePending) {
                performUpdate()
            }
        }
    }

    Connections {
        target: dbusproxy

        onProvideDiagnosisKeysResult: {
            var summary
            if (token == page.token) {
                updating = false;

                if (status == DBusProxy.Success) {
                    console.log("Exposure summary")
                    summary = dbusproxy.getExposureSummary(token);
                    console.log("Attenuation durations: " + summary.attenuationDurations)
                    console.log("Days since last exposure: " + summary.daysSinceLastExposure)
                    console.log("Matched key count: " + summary.matchedKeyCount)
                    console.log("Maximum risk score: " + summary.maximumRiskScore)
                    console.log("Summation risk score: " + summary.summationRiskScore)
                    Settings.summaryUpdated = new Date()
                    Settings.latestSummary = summary
                }
            }
        }
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
                //% "Contrac Exposure Notification"
                title: qsTrId("contrac-main_title")
            }

            SectionHeader {
                //% "Status"
                text: qsTrId("contrac-main_he_status")
            }

            Item {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                height: Theme.itemSizeSmall

                BusyIndicator {
                    id: progress
                    anchors.verticalCenter: parent.verticalCenter
                    running: visible
                    size: BusyIndicatorSize.Small
                    visible: busy
                }

                Image {
                    anchors.fill: progress
                    visible: !progress.visible
                    source: {
                        if (upload.status === Upload.StatusError) {
                            return Qt.resolvedUrl("image://contrac/icon-s-warning")
                        } else if (upload.status === Upload.StatusError) {
                            return Qt.resolvedUrl("image://contrac/icon-s-warning")
                        } else if (download.status === Download.StatusError) {
                            return Qt.resolvedUrl("image://contrac/icon-s-warning")
                        } else if (Settings.latestSummary.summationRiskScore >= 15) {
                            return Qt.resolvedUrl("image://contrac/icon-s-warning")
                        } else if (downloadAvailable) {
                            return Qt.resolvedUrl("image://contrac/icon-s-unknown")
                        } else if (!dbusproxy.isEnabled) {
                            return Qt.resolvedUrl("image://contrac/icon-s-inactive")
                        } else {
                            return Qt.resolvedUrl("image://contrac/icon-s-active")
                        }
                    }
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
                        if (updating) {
                            //% "Updating"
                            return qsTrId("contrac-main_la_status-updating")
                        } else if (upload.uploading) {
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
                            return qsTrId("contrac-main_la_status-download_error")
                        } else if (dbusproxy.isBusy) {
                            //% "Busy"
                            return qsTrId("contrac-main_la_status-busy")
                        } else if (Settings.latestSummary.summationRiskScore >= 15) {
                            //% "At risk"
                            return qsTrId("contrac-main_la_status-daily-update-required")
                        } else if (downloadAvailable) {
                            //% "Daily update required"
                            return qsTrId("contrac-main_la_status-at-risk")
                        } else if (dbusproxy.isEnabled) {
                            //% "Active"
                            return qsTrId("contrac-main_la_status-active")
                        } else {
                            //% "Disabled"
                            return qsTrId("contrac-main_la_status-disabled")
                        }
                    }
                    color: Theme.highlightColor

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            download.clearError()
                            upload.clearError()
                        }
                    }
                }
            }

            Label {
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                //% "Risk status"
                text: qsTrId("contrac-main_la_risk-status") + " : " + (!isNaN(Settings.summaryUpdated)
                                                                       ? translateRiskScore(Settings.latestSummary.summationRiskScore)
                                                                         //% "Unknown"
                                                                       : qsTrId("contrac-main_la_risk-status-unknown"))
                color: Theme.highlightColor
                wrapMode: Text.Wrap
            }

            Label {
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                //% "Latest update"
                text: qsTrId("contrac-main_la_last-update") + " : " + (!isNaN(Settings.summaryUpdated)
                                                                       ? Qt.formatDateTime(Settings.summaryUpdated, "d MMM yyyy, hh:mm")
                                                                         //% "Never"
                                                                       : qsTrId("contrac-main_la_latest-update-never"))
                color: Theme.highlightColor
                wrapMode: Text.Wrap
            }

            Label {
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                //% "Days since last exposure"
                text: qsTrId("contrac-main_la_days-since-last-exposure") + " : " + (Settings.latestSummary.matchedKeyCount > 0
                                                                                    ? Settings.latestSummary.daysSinceLastExposure
                                                                                      //% "None recorded"
                                                                                    : qsTrId("contrac-main_la_days-since-last-exposure-none"))
                color: Theme.highlightColor
                wrapMode: Text.Wrap
            }

            Label {
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                //% "Number of matched keys"
                text: qsTrId("contrac-main_la_matched-keys") + " : " + Settings.latestSummary.matchedKeyCount
                color: Theme.highlightColor
                wrapMode: Text.Wrap
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
                //% "Control"
                text: qsTrId("contrac-main_bt_actions")
            }

            TextSwitch {
                //% "Scan and send active"
                text: qsTrId("contrac-main_scan")
                width: parent.width - 2 * Theme.horizontalPageMargin
                checked: dbusproxy.isEnabled
                busy: dbusproxy.isBusy
                automaticCheck: false
                onClicked: triggerEnabled()
            }

            Button {
                id: tanButton
                anchors.horizontalCenter: parent.horizontalCenter
                width: Math.max(tanButton.implicitWidth, downloadButton.implicitWidth)
                //% "Enter TeleTAN"
                text: qsTrId("contrac-main_bu_enter-teletan")
                enabled: !upload.uploading
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("TeleTAN.qml"))
                }
            }

            Button {
                id: downloadButton
                anchors.horizontalCenter: parent.horizontalCenter
                width: Math.max(tanButton.implicitWidth, downloadButton.implicitWidth)
                enabled: !download.downloading && downloadAvailable
                //% "Perform daily update"
                text: qsTrId("contrac-main_bu_daily-update")
                onClicked: {
                    download.downloadLatest()
                    pageStack.push(Qt.resolvedUrl("DownloadInfo.qml"))
                }
            }
        }
    }
}
