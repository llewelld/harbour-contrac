import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"
import uk.co.flypig.contrac 1.0

Page {
    id: infoPage

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
                //% "Important information"
                title: qsTrId("contrac-info_title")
            }

            Label {
                //% "Contrac is now using Germany's official Corona-Warn-App servers for both downloading "
                //% "diagnosis keys from other users and uploading your diagnosis keys using a TeleTAN if "
                //% "your test results return positive."
                //% "<br/><br/>"
                //% "Sailfish OS users who downloaded the previous versions of the app have been "
                //% "testing it against the test servers made available by the Corona-Warn-App project. "
                //% "This doesn't guarantee the functionality will work correctly using the real "
                //% "servers and unfortunately there's no way to fully test this except to use the app."
                //% "<br/><br/>"
                //% "The Corona-Warn-App team can only support their official iOS and Android apps, "
                //% "so it's important to understand that if you use Contrac there is no guarantee "
                //% "it'll work as expected."
                //% "<br/><br/>"
                //% "If you do experience issues, or even if things work as expected, please do share "
                //% "your experiences."
                //% "<br/><br/>"
                //% "Thank you for trying the app!"
                text: qsTrId("contrac-info_main-text")
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

            GlassItem {
                height: Theme.paddingLarge
                width: parent.width
                falloffRadius: 0.15
                radius: 0.15
                color: Theme.highlightColor
                cache: false
            }

            Label {
                //% "You should configure the Bluetooth attenuation calibration values before using this app. Please visit the Settings page to do this."
                text: qsTrId("contrac-info_configure-attenuation-calibration-values")
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

            GlassItem {
                height: Theme.paddingLarge
                width: parent.width
                falloffRadius: 0.15
                radius: 0.15
                color: Theme.highlightColor
                cache: false
            }

            Label {
                //% "You can view this page again by visiting the About page."
                text: qsTrId("contrac-info_view-again")
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
        }
    }
}
