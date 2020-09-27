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
                //% "About Contrac"
                title: qsTrId("contrac-about_title")
            }

            Image {
                anchors.topMargin: Theme.paddingLarge
                anchors.horizontalCenter: parent.horizontalCenter
                source  : Qt.resolvedUrl("image://contrac/contrac-title")
                width: parent.width - (2 * Theme.horizontalPageMargin)
                height: Theme.iconSizeExtraLarge
                fillMode: Image.PreserveAspectFit
            }

            Label {
                //% "Corona-Warn-App compatible contact Tracing using the Apple/Google Exposure Notification API"
                text: qsTrId("contrac-about_description")
                wrapMode: Text.WordWrap
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.highlightColor
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
                // The version property is set in the application
                // initialisation code in harbour-contrac.cpp
                value: version
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
                //% "Contributors"
                text: qsTrId("contrac-about_he_contributors")
            }

            Label {
                //% "ApostolosB, dashinfantry, flypig, smdesai, spodermenpls, Thaodan"
                text: qsTrId("contrac-about_la_contributors")
                wrapMode: Text.WordWrap
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.highlightColor
                anchors {
                    leftMargin: Theme.horizontalPageMargin
                    rightMargin: Theme.horizontalPageMargin
                    left: parent.left
                    right: parent.right
                }
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

            SectionHeader {
                //% "Information"
                text: qsTrId("contrac-about_header_info")
            }

            Row {
                spacing: Theme.paddingLarge
                anchors.horizontalCenter: parent.horizontalCenter
                Button {
                    id: infoPage
                    //% "Info page"
                    text: qsTrId("contrac-about_info-page")
                    enabled: true
                    onClicked: pageStack.push(Qt.resolvedUrl("Info.qml"))
                }
            }
        }
    }
}
