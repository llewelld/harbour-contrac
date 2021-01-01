import QtQuick 2.4
import Sailfish.Silica 1.0


Rectangle {
    property string tan
    property int index
    property bool set: index < tan.length
    property bool valid: set && "0123456789ABCDEF".indexOf(character) >= 0
    property string character: set ? tan.charAt(index).toUpperCase() : ""

    width: textMetrics.width + 2 * Theme.paddingSmall
    height: textMetrics.height
    border.color: set ? Theme.secondaryHighlightColor : Theme.highlightColor
    color: "transparent"

    TextMetrics {
        id: textMetrics
        text: "M"
        font.pixelSize: Theme.fontSizeHuge
    }

    Label {
        id: label
        text: set ? character : "_"
        anchors.centerIn: parent
        font.pixelSize: Theme.fontSizeHuge
        color: !set || valid ? Theme.primaryColor : Theme.errorColor
    }
}

