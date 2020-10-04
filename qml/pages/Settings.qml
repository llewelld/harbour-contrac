import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"
import uk.co.flypig.contrac 1.0

Page {
    id: settingsPage

    Binding { target: AppSettings; property: "downloadServer"; value: downloadServerEntry.text }
    Binding { target: AppSettings; property: "uploadServer"; value: uploadServerEntry.text }
    Binding { target: AppSettings; property: "verificationServer"; value: verificationServerEntry.text }
    Binding { target: dbusproxy; property: "txPower"; value: txPowerEntry.text }
    Binding { target: dbusproxy; property: "rssiCorrection"; value: rssiCorrectionEntry.text }

    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: settingsColumn.height + Theme.paddingLarge
        flickableDirection: Flickable.VerticalFlick

        VerticalScrollDecorator {}

        Column {
            id: settingsColumn
            width: parent.width
            spacing: Theme.paddingLarge

            PageHeader {
                //% "Settings"
                title: qsTrId("contrac-settings_he_settings")
            }

            SectionHeader {
                //% "Servers"
                text: qsTrId("contrac-settings_he_servers")
            }

            TextField {
                id: downloadServerEntry
                //% "Download server"
                label: qsTrId("contrac-settings_tf_download_server")
                placeholderText: label
                width: parent.width
                text: AppSettings.downloadServer
                inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.onClicked: uploadServerEntry.focus = true
            }

            TextField {
                id: uploadServerEntry
                //% "Upload server"
                label: qsTrId("contrac-settings_tf_upload_server")
                placeholderText: label
                width: parent.width
                text: AppSettings.uploadServer
                inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.onClicked: verificationServerEntry.focus = true
            }

            TextField {
                id: verificationServerEntry
                //% "Verification server"
                label: qsTrId("contrac-settings_tf_verification_server")
                placeholderText: label
                width: parent.width
                text: AppSettings.verificationServer
                inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.onClicked: txPowerEntry.focus = true
            }

            SectionHeader {
                //% "Attenuation calibration"
                text: qsTrId("contrac-settings_he_attentuation-values")
            }

            TextField {
                id: txPowerEntry
                //% "Transmit power"
                label: qsTrId("contrac-settings_tf_transmit-power")
                placeholderText: label
                width: parent.width
                text: dbusproxy.txPower
                inputMethodHints: Qt.ImhDigitsOnly
                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.onClicked: rssiCorrectionEntry.focus = true
            }

            TextField {
                id: rssiCorrectionEntry
                //% "RSSI correction"
                label: qsTrId("contrac-settings_tf_rssi-correction")
                placeholderText: label
                width: parent.width
                text: dbusproxy.rssiCorrection
                inputMethodHints: Qt.ImhDigitsOnly
                //EnterKey.iconSource: "image://theme/icon-m-enter-next"
                //EnterKey.onClicked: Entry.focus = true
            }

            Label {
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                color: Theme.highlightColor
                wrapMode: Text.Wrap
                //% "Download the calibration spreadsheet to check the attenuation values for your device."
                text: qsTrId("contrac-settings_la_attenuation_info")
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                //% "Download"
                text: qsTrId("contrac-settings_bu_download_attenuation_spreadsheet")
                onClicked: Qt.openUrlExternally("https://developers.google.com/android/exposure-notifications/files/en-calibration-2020-08-12.csv")
            }
        }
    }
}
