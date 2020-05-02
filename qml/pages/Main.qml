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

    BleScanner {
        id: blescanner
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

            Row {
                spacing: Theme.paddingLarge
                width: parent.width
                height: Theme.itemSizeSmall

                TextSwitch {
                    text: "Scan"
                    width: (parent.width / 2.0) - parent.spacing
                    automaticCheck: false;
                    checked: blescanner.scan
                    busy: blescanner.busy
                    onClicked: {
                        blescanner.scan = !checked
                    }
                }

                TextSwitch {
                    text: "Transmit"
                    width: (parent.width / 2.0) - parent.spacing
                    automaticCheck: false;
                    checked: controller.active
                    onClicked: {
                        if (controller.active) {
                            controller.unRegisterAdvert();
                        }
                        else {
                            controller.registerAdvert();
                        }
                    }
                }
            }


            Label {
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                //% "Date"
                text: qsTrId("contrac-main_date") + ": " + Qt.formatDateTime(contrac.time, "yyyy-MM-dd hh:mm:ss")
                color: Theme.highlightColor
            }

            Row {
                spacing: Theme.paddingLarge
                width: parent.width - 2 * Theme.horizontalPageMargin
                height: Theme.itemSizeSmall
                x: Theme.horizontalPageMargin

                Label {
                    width: (parent.width / 2) - parent.spacing
                    //% "Day number"
                    text: qsTrId("contrac-main_dn") + ": " + contrac.dayNumber
                    color: Theme.highlightColor
                }

                Label {
                    width: (parent.width / 2) - parent.spacing
                    //% "Time number"
                    text: qsTrId("contrac-main_tin") + ": " + contrac.timeIntervalNumber
                    color: Theme.highlightColor
                }
            }

            Label {
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                //% "Tracing Key"
                text: qsTrId("contrac-main_tk")
                color: Theme.highlightColor
            }

            Label {
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                text: controller.binaryToHex(contrac.tk, 24)
                font.family: "Monospace"
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.highlightColor
            }

            Label {
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                //% "Daily Tracing Key"
                text: qsTrId("contrac-main_dtk")
                color: Theme.highlightColor
            }

            Label {
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                text: controller.binaryToHex(contrac.dtk, 24)
                font.family: "Monospace"
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.highlightColor
            }

            Label {
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                //% "Random Proximity Identifer"
                text: qsTrId("contrac-main_rpi")
                color: Theme.highlightColor
            }

            Label {
                id: rpiBytes
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                text: controller.binaryToHex(contrac.rpi, 24)
                font.family: "Monospace"
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.highlightColor

                ColorAnimation on color {
                    id: rpiColourAnimation
                    from: Theme.errorColor
                    to: Theme.highlightColor
                    duration: 500
                    running: true
                }
            }
        }
    }
}
