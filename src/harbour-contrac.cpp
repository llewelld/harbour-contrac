/*
 * Copyright 2020, David Llewellyn-Jones <david@flypig.co.uk>
 * Released under the GPLv2
 *
 * More info at: https://www.flypig.co.uk/contrac
 *
 */

#include <QtQuick>
#include <openssl/rand.h>

#include "dbusproxy.h"
#include "download.h"
#include "upload.h"
#include "settings.h"

#include <sailfishapp.h>

int main(int argc, char *argv[])
{
    // SailfishApp::main() will display "qml/ble.qml", if you need more
    // control over initialization, you can use:
    //
    //   - SailfishApp::application(int, char *[]) to get the QGuiApplication *
    //   - SailfishApp::createView() to get a new QQuickView * instance
    //   - SailfishApp::pathTo(QString) to get a QUrl to a resource file
    //   - SailfishApp::pathToMainQml() to get a QUrl to the main QML file
    //
    // To display the view, call "show()" (will show fullscreen on device).

    QGuiApplication *app = SailfishApp::application(argc, argv);
    QCoreApplication::setOrganizationDomain("www.flypig.co.uk");
    QCoreApplication::setApplicationName("harbour-contrac");

    Settings::instantiate();

    qmlRegisterType<DBusProxy>("uk.co.flypig.contrac", 1, 0, "DBusProxy");
    qmlRegisterType<TemporaryExposureKey>("uk.co.flypig.contrac", 1, 0, "TemporaryExposureKey");
    qmlRegisterType<ExposureSummary>("uk.co.flypig.contrac", 1, 0, "ExposureSummary");
    qmlRegisterType<ExposureInformation>("uk.co.flypig.contrac", 1, 0, "ExposureInformation");
    qmlRegisterType<ExposureConfiguration>("uk.co.flypig.contrac", 1, 0, "ExposureConfiguration");
    qmlRegisterType<Download>("uk.co.flypig.contrac", 1, 0, "Download");
    qmlRegisterType<Upload>("uk.co.flypig.contrac", 1, 0, "Upload");

    qmlRegisterSingletonType<Settings>("uk.co.flypig.contrac", 1, 0, "Settings", Settings::provider);

    QQuickView *view = SailfishApp::createView();
    view->setSource(SailfishApp::pathTo("qml/harbour-contrac.qml"));

    view->show();

    int result = app->exec();

    qDebug() << "Execution finished: " << result;
    delete view;
    qDebug() << "Deleted view";
    delete app;
    qDebug() << "Deleted app";

    return result;
}
