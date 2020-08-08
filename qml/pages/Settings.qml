import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"
import uk.co.flypig.contrac 1.0

Page {
    id: settingsPage

    Binding { target: Settings; property: "downloadServer"; value: downloadServerEntry.text }
    Binding { target: Settings; property: "uploadServer"; value: uploadServerEntry.text }
    Binding { target: Settings; property: "verificationServer"; value: verificationServerEntry.text }

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
                //% "Servers"
                title: qsTrId("contrac-settings_he_servers")
            }

            TextField {
                id: downloadServerEntry
                //% "Download server"
                label: qsTrId("contrac-settings_tf_download_server")
                placeholderText: label
                width: parent.width
                text: Settings.downloadServer
                inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.onClicked: uploadServer.focus = true
            }

            TextField {
                id: uploadServerEntry
                //% "Upload server"
                label: qsTrId("contrac-settings_tf_upload_server")
                placeholderText: label
                width: parent.width
                text: Settings.uploadServer
                inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.onClicked: verificationServer.focus = true
            }

            TextField {
                id: verificationServerEntry
                //% "Verification server"
                label: qsTrId("contrac-settings_tf_verification_server")
                placeholderText: label
                width: parent.width
                text: Settings.verificationServer
                inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                //EnterKey.iconSource: "image://theme/icon-m-enter-next"
                //EnterKey.onClicked: uploadUrl.focus = true
            }
        }
    }
}
