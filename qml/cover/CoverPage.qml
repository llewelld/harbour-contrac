import QtQuick 2.0
import Sailfish.Silica 1.0
import uk.co.flypig.contrac 1.0

CoverBackground {
    Image {
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width
        height: sourceSize.height * width / sourceSize.width
        source: Qt.resolvedUrl("image://contrac/cover-background")
        opacity: 0.1
    }

    Label {
        id: dummyLetter
        text: ""
        y: Theme.paddingMedium
        x: Theme.paddingLarge
        width: height
        font.pixelSize: Theme.fontSizeHuge
        horizontalAlignment: Text.AlignRight
    }

    BusyIndicator {
        id: progress
        running: visible
        size: BusyIndicatorSize.Medium
        visible: busy
        _forceAnimation: visible
        anchors.centerIn: statusImage
    }

    Image {
        id: statusImage
        anchors {
            top: dummyLetter.top
            right: parent.right
            rightMargin: Theme.paddingLarge
        }
        width: dummyLetter.width
        height: dummyLetter.height
        visible: !progress.visible
        source: {
            if (upload.status === Upload.StatusError) {
                return Qt.resolvedUrl("image://contrac/cover-warning")
            } else if (upload.status === Upload.StatusError) {
                return Qt.resolvedUrl("image://contrac/cover-warning")
            } else if (download.status === Download.StatusError) {
                return Qt.resolvedUrl("image://contrac/cover-warning")
            } else if (riskStatus.riskClassIndex > 0) {
                return Qt.resolvedUrl("image://contrac/cover-warning")
            } else if (downloadAvailable) {
                return Qt.resolvedUrl("image://contrac/cover-unknown")
            } else if (!dbusproxy.isEnabled) {
                return Qt.resolvedUrl("image://contrac/cover-inactive")
            } else {
                return Qt.resolvedUrl("image://contrac/cover-active")
            }
        }
    }

    Label {
        id: riskLevel
        //% "Current"
        text: qsTrId("contrac-cover_current-status-line1") + "\n"
        //% "Status"
        + qsTrId("contrac-cover_current-status-line2")
        font.pixelSize: Theme.fontSizeExtraSmall
        maximumLineCount: 2
        wrapMode: Text.Wrap
        fontSizeMode: Text.HorizontalFit
        lineHeight: 0.8
        height: implicitHeight/0.8
        verticalAlignment: Text.AlignVCenter
        anchors {
            right: statusImage.right
            left: dummyLetter.left
            baseline: dummyLetter.baseline
            baselineOffset: lineCount > 1 ? -implicitHeight/2 : -(height-implicitHeight)/2
        }
    }

    Label {
        id: statusLabel

        visible: true
        x: Theme.paddingLarge
        color: Theme.highlightColor
        //% "Contrac"
        text: qsTrId("contrac-cover_la_title")
        fontSizeMode: Text.VerticalFit
        font.pixelSize: Theme.fontSizeLarge
        width: parent.width - (2 * Theme.paddingLarge)
        anchors {
            top: statusImage.bottom
            topMargin: Theme.paddingSmall
        }
    }

    CoverActionList {
        id: coverActionWithDownload
        enabled: downloadAvailable && !busy

        CoverAction {
            iconSource: dbusproxy.isEnabled ? AppSettings.getImageUrl("cover-action-active") : AppSettings.getImageUrl("cover-action-inactive")
            onTriggered: triggerEnabled()
        }

        CoverAction {
            iconSource: Qt.resolvedUrl("image://theme/icon-cover-refresh")
            onTriggered: download.downloadLatest()
        }
    }

    CoverActionList {
        id: coverActionWithoutDownload
        enabled: !coverActionWithDownload.enabled

        CoverAction {
            iconSource: dbusproxy.isEnabled ? AppSettings.getImageUrl("cover-action-active") : AppSettings.getImageUrl("cover-action-inactive")
            onTriggered: triggerEnabled()
        }
    }
}
