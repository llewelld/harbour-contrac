import QtQuick 2.4
import Sailfish.Silica 1.0


Item {
    width: textMetrics.width + 2 * Theme.paddingSmall
    height: textMetrics.height + 2 * Theme.paddingSmall

    TextMetrics {
        id: textMetrics
        text: "-"
        font.pixelSize: Theme.fontSizeHuge
    }

    Label {
        text: "-"
        anchors.centerIn: parent
        font.pixelSize: Theme.fontSizeHuge
    }
}

