import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"
import uk.co.flypig.contrac 1.0

Page {
    id: aboutPage

    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: aboutColumn.height + Theme.paddingLarge
        flickableDirection: Flickable.VerticalFlick

        VerticalScrollDecorator {}

        Column {
            id: aboutColumn
            width: parent.width
            spacing: Theme.paddingLarge

            PageHeader {
                //% "About Contact Tracer"
                title: qsTrId("contrac-about_title")
            }

            Label {
                //% "Demonstration of the Apple/Google Contract Tracing API"
                text: qsTrId("contrac-about_description")
                wrapMode: Text.WordWrap
                font.pixelSize: Theme.fontSizeSmall
                anchors {
                    leftMargin: Theme.horizontalPageMargin
                    rightMargin: Theme.horizontalPageMargin
                    left: parent.left
                    right: parent.right
                }
            }

            InfoRow {
                //% "Version"
                label: qsTrId("contrac-about_verion")
                value: "0.0.1"
                midlineRatio: 0.4
                midlineMin: Theme.fontSizeSmall * 5
                midlineMax: Theme.fontSizeSmall * 10
            }

            InfoRow {
                //% "App author"
                label: qsTrId("contrac-about_app_author")
                //% "David Llewellyn-Jones"
                value: qsTrId("contrac-about_app_author_name")
                midlineRatio: 0.4
                midlineMin: Theme.fontSizeSmall * 5
                midlineMax: Theme.fontSizeSmall * 10
            }

            InfoRow {
                //% "Licence"
                label: qsTrId("contrac-about_licence")
                //% "GPLv2"
                value: qsTrId("contrac-about_gpl_v2")
                midlineRatio: 0.4
                midlineMin: Theme.fontSizeSmall * 5
                midlineMax: Theme.fontSizeSmall * 10
            }

            SectionHeader {
                //% "Links"
                text: qsTrId("contrac-about_header_links")
            }

            Row {
                spacing: Theme.paddingLarge
                anchors.horizontalCenter: parent.horizontalCenter
                Button {
                    id: website
                    //% "Website"
                    text: qsTrId("contrac-about_website")
                    enabled: true
                    onClicked: Qt.openUrlExternally("https://www.flypig.co.uk/contrac")
                }
                Button {
                    id : email
                    //% "Email"
                    text: qsTrId("contrac-about_email")
                    enabled: true
                    onClicked: Qt.openUrlExternally("mailto:david@flypig.co.uk")
                }
            }
        }
    }
}
