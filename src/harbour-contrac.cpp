/*
 * Copyright 2020, David Llewellyn-Jones <david@flypig.co.uk>
 * Released under the GPLv2
 *
 * More info at: https://www.flypig.co.uk/contrac
 *
 */

#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include "controller.h"
#include "contrac.h"
#include "bleascanner.h"
#include "contactmodel.h"
#include "contactstorage.h"

#include "bloomfilter.h"
#include <openssl/rand.h>

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

    qmlRegisterType<Controller>("uk.co.flypig", 1, 0, "Controller");
    qmlRegisterType<Contrac>("uk.co.flypig", 1, 0, "Contrac");
    qmlRegisterType<BleScanner>("uk.co.flypig", 1, 0, "BleScanner");
    qmlRegisterType<ContactModel>("uk.co.flypig", 1, 0, "ContactModel");
    qmlRegisterType<ContactStorage>("uk.co.flypig", 1, 0, "ContactStorage");

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
