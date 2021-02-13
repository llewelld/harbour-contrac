/*
 * Copyright 2020, David Llewellyn-Jones <david@flypig.co.uk>
 * Released under the GPLv2
 *
 * More info at: https://www.flypig.co.uk/contrac
 *
 */

#include <openssl/rand.h>
#include <QtQuick>

#include "appsettings.h"
#include "autoupdate.h"
#include "dbusproxy.h"
#include "download.h"
#include "downloadconfig.h"
#include "imageprovider.h"
#include "riskstatus.h"
#include "upload.h"
#include "../contracd/src/version.h"

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
    QCoreApplication::setOrganizationName("harbour-contrac");
    QCoreApplication::setApplicationName("harbour-contrac");

    qDebug() << "Contrac version" << VERSION;

    // Needed for Settings save/load
    qRegisterMetaType<ExposureSummary>();
    qRegisterMetaType<RiskScoreClass>();
    qRegisterMetaTypeStreamOperators<ExposureSummary>("ExposureSummary");
    qRegisterMetaTypeStreamOperators<RiskScoreClass>("RiskScoreClass");

    AppSettings::instantiate(app);

    qmlRegisterType<DBusProxy>("uk.co.flypig.contrac", 1, 0, "DBusProxy");
    qmlRegisterType<TemporaryExposureKey>("uk.co.flypig.contrac", 1, 0, "TemporaryExposureKey");
    qmlRegisterType<ExposureSummary>("uk.co.flypig.contrac", 1, 0, "ExposureSummary");
    qmlRegisterType<ExposureInformation>("uk.co.flypig.contrac", 1, 0, "ExposureInformation");
    qmlRegisterType<ExposureConfiguration>("uk.co.flypig.contrac", 1, 0, "ExposureConfiguration");
    qmlRegisterType<Download>("uk.co.flypig.contrac", 1, 0, "Download");
    qmlRegisterType<Upload>("uk.co.flypig.contrac", 1, 0, "Upload");
    qmlRegisterType<DownloadConfig>("uk.co.flypig.contrac", 1, 0, "DownloadConfig");
    qmlRegisterType<RiskStatus>("uk.co.flypig.contrac", 1, 0, "RiskStatus");
    qmlRegisterType<AutoUpdate>("uk.co.flypig.contrac", 1, 0, "AutoUpdate");

    qmlRegisterSingletonType<AppSettings>("uk.co.flypig.contrac", 1, 0, "AppSettings", AppSettings::provider);

    QQuickView *view = SailfishApp::createView();
    // The engine takes ownership of the ImageProvider
    view->engine()->addImageProvider(QLatin1String("contrac"), new ImageProvider(AppSettings::getInstance()));
    view->setSource(SailfishApp::pathTo("qml/harbour-contrac.qml"));
    QQmlContext *ctxt = view->rootContext();
    ctxt->setContextProperty("version", VERSION);

    view->show();

    int result = app->exec();

    qDebug() << "Execution finished:" << result;
    delete view;
    qDebug() << "Deleted view";
    delete app;
    qDebug() << "Deleted app";

    return result;
}
