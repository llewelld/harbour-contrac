import QtQuick 2.0
import Sailfish.Silica 1.0
import uk.co.flypig 1.0

Page {
    id: page

    allowedOrientations: Orientation.All

    Controller {
        id: controller
    }

    Contrac {
        id: contrac

        onRpiChanged: {
            controller.setRpi(contrac.rpi)
            if (controller.active) {
                rpiColourAnimation.start()
            }
        }
    }

    Timer {
        // Update at least every Daily Time Number interval (10 mins)
        interval: 0.025 * 1000
        repeat: true
        running: true
        onTriggered: {
            contrac.updateKeys()
        }
    }

    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                //% "About"
                text: qsTrId("contrac-main_about")
                onClicked: pageStack.push(Qt.resolvedUrl("About.qml"))
            }
        }

        contentHeight: column.height

        Column {
            id: column

            width: page.width
            spacing: Theme.paddingLarge
            PageHeader {
                //% "BLE Contact Tracing"
                title: qsTrId("contrac-main_title")
            }

            SectionHeader {
                //% "Time"
                text: qsTrId("contrac-main_time")
            }

            Label {
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                //% "Date"
                text: qsTrId("contrac-main_date") + ": " + Qt.formatDateTime(contrac.time, "yyyy-MM-dd hh:mm:ss")
                color: Theme.highlightColor
            }

            Label {
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                //% "Day number"
                text: qsTrId("contrac-main_dn") + ": " + contrac.dayNumber
                color: Theme.highlightColor
            }

            Label {
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                //% "Time interval number"
                text: qsTrId("contrac-main_tin") + ": " + contrac.timeIntervalNumber
                color: Theme.highlightColor
            }

            SectionHeader {
                //% "Tracing Key"
                text: qsTrId("contrac-main_tk")
            }

            Label {
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                text: controller.binaryToHex(contrac.tk, 16)
                font.family: "Monospace"
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.highlightColor
            }

            SectionHeader {
                //% "Daily Tracing Key"
                text: qsTrId("contrac-main_dtk")
            }

            Label {
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                text: controller.binaryToHex(contrac.dtk, 16)
                font.family: "Monospace"
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.highlightColor
            }

            SectionHeader {
                //% "Random Proximity Identifer"
                text: qsTrId("contrac-main_rpi")
            }

            Label {
                id: rpiBytes
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                text: controller.binaryToHex(contrac.rpi, 16)
                font.family: "Monospace"
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.highlightColor

                ColorAnimation on color {
                    id: rpiColourAnimation
                    from: Theme.errorColor
                    to: Theme.highlightColor
                    duration: 500
                    running: true
                }
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                //% "Activate"
                text: qsTrId("contrac-main_activate")
                onClicked: {
                    controller.registerAdvert();
                    rpiColourAnimation.start()
                }
                enabled: !controller.active
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter

                //% "Deactivate"
                text: qsTrId("contrac-main_deactivate")
                onClicked: {
                    controller.unRegisterAdvert();
                    rpiColourAnimation.start()
                }
                enabled: controller.active
            }
        }
    }
}
