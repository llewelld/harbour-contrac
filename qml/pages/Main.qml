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
        }
    }

    BleScanner {
        id: blescanner
        onBeaconDiscovered: {
            contactModel.addContact(address, rpi, rssi)
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

    Timer {
        interval: 5 * 1000
        repeat: true
        running: true
        onTriggered: contactModel.harvestOldContacts()
    }

    SilicaListView {
        anchors.fill: parent
        model: ContactModel {
            id: contactModel
        }

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

            Row {
                spacing: Theme.paddingLarge
                width: parent.width
                height: Theme.itemSizeSmall

                TextSwitch {
                    //% "Scan"
                    text: qsTrId("contrac-main_scan")
                    width: (parent.width / 2.0) - parent.spacing
                    automaticCheck: false;
                    checked: blescanner.scan
                    busy: blescanner.busy
                    onClicked: {
                        blescanner.scan = !checked
                    }
                }

                TextSwitch {
                    //% "Transmit"
                    text: qsTrId("contrac-main_transmit")
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
                height: dayNumber.height
                x: Theme.horizontalPageMargin

                Label {
                    id: dayNumber
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
                text: controller.binaryToHex(contrac.dtk, 16)
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
                text: controller.binaryToHex(contrac.rpi, 16)
                font.family: "Monospace"
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.highlightColor
                onTextChanged: {
                    if (controller.active) {
                        rpiColourAnimation.start();
                    }
                }

                ColorAnimation on color {
                    id: rpiColourAnimation
                    from: Theme.errorColor
                    to: Theme.highlightColor
                    duration: 500
                    running: true
                }
            }

            SectionHeader {
                //% "Beacons received"
                text: qsTrId("contrac-main_received");
            }
        }

        add: Transition {
            SequentialAnimation {
                NumberAnimation {
                    properties: "opacity"
                    from: 0.0
                    to: 1.0
                    duration: 200
                    easing.type: Easing.OutCubic
                }
                ColorAnimation {
                    properties: "color"
                    from: Theme.errorColor
                    to: Theme.highlightColor
                    duration: 500
                }
            }
        }

        displaced: Transition {
            NumberAnimation {
                properties: "y"
                duration: 100
                easing.type: Easing.OutCubic
            }
        }

        remove: Transition {
            NumberAnimation {
                properties: "opacity"
                to: 0
                duration: 2000
                easing.type: Easing.OutCubic
            }
        }

        delegate: ListItem {
            property alias color: receivedRpi.color
            contentHeight: Theme.itemSizeSmall
            Row {
                spacing: Theme.paddingLarge
                height: Theme.itemSizeSmall
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                Label {
                    id: receivedRpi
                    text: controller.binaryToHex(model.rpi, 16)
                    //width: parent.width - 2 * Theme.horizontalPageMargin
                    font.family: "Monospace"
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: Theme.errorColor
                }

                Label {
                    width: parent.width - receivedRpi.width - Theme.paddingLarge
                    text: model.rssi
                    horizontalAlignment: Label.AlignRight
                    color: Theme.highlightColor
                }
            }
        }
    }
}
