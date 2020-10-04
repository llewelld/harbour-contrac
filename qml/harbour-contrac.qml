import QtQuick 2.0
import Sailfish.Silica 1.0
import org.nemomobile.time 1.0
import uk.co.flypig.contrac 1.0
import "pages"

ApplicationWindow
{
    readonly property bool downloadAvailable: moreThanADayAgo(AppSettings.summaryUpdated)
    property bool updating
    readonly property bool busy: upload.uploaindg || download.downloading || updating || dbusproxy.isBusy

    DBusProxy {
        id: dbusproxy
    }

    Upload {
        id: upload
    }

    Download {
        id: download
    }

    WallClock {
        id: wallclock
        updateFrequency: WallClock.Day
    }

    RiskStatus {
        id: riskStatus
    }

    function moreThanADayAgo(latest) {
        var result = true
        if (!isNaN(latest)) {
            var today = wallclock.time
            today.setSeconds(0)
            today.setMinutes(0)
            today.setHours(0)
            today.setMilliseconds(0)
            latest.setSeconds(0)
            latest.setMinutes(0)
            latest.setHours(0)
            latest.setMilliseconds(0)
            result = ((today - latest) >= (24 * 60 * 60 * 1000))
        }
        return result
    }

    function triggerEnabled() {
        if (!dbusproxy.isBusy) {
            if (dbusproxy.isEnabled) {
                console.log("Clicked to stop")
                dbusproxy.stop();
            }
            else {
                console.log("Clicked to start")
                dbusproxy.start();
            }
        }
        else {
            consloe.log("Can't start or stop while busy")
        }
    }

    initialPage: Component { Main { } }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations
}
