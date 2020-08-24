/*
 * Copyright 2020, David Llewellyn-Jones <david@flypig.co.uk>
 * Released under the GPLv2
 *
 * More info at: https://www.flypig.co.uk/contrac
 *
 */

#include <QtCore/QCoreApplication>
#include <QtQuick>
#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>

#include "dbusinterface.h"
#include "settings.h"
#include "contracd.h"

static void signal_handler(int sig);

int main(int argc, char *argv[])
{
    int result;
    DBusInterface *dbus = nullptr;

    QCoreApplication *app = new QCoreApplication(argc, argv);
    QCoreApplication::setOrganizationDomain("www.flypig.co.uk");
    QCoreApplication::setOrganizationName("contracd");
    QCoreApplication::setApplicationName("contracd");

    qDebug() << "contrac VERSION string:" << VERSION;
    qDebug() << "VERSION_MAJOR:" << VERSION_MAJOR;
    qDebug() << "VERSION_MINOR:" << VERSION_MINOR;
    qDebug() << "VERSION_BUILD:" << VERSION_BUILD;

    Settings::instantiate(app);

    umask(0);

    result = chdir("/tmp");

    if (result >= 0) {
        signal(SIGHUP, signal_handler);
        signal(SIGTERM, signal_handler);
        signal(SIGINT, signal_handler);

        setlinebuf(stdout);
        setlinebuf(stderr);

        dbus = new DBusInterface();
        result = dbus == nullptr ? 0 : 1;
    }
    else {
        qDebug() << "No /tmp directory";
    }

    if (result >= 0) {
        qDebug() << "Execution started";
        result = app->exec();
    }
    else {
        qDebug() << "DBusInterface could not be created";
    }

    if (dbus) {
        delete dbus;
    }

    qDebug() << "Execution finished:" << result;
    delete app;
    qDebug() << "Deleted app";

    return result;
}

static void signal_handler(int sig)
{
    switch (sig) {
    case SIGHUP:
        printf("Received SIGHUP signal\n");
        break;
    case SIGTERM:
        printf("Received SIGTERM signal\n");
        QCoreApplication::quit();
        break;
    case SIGINT:
        printf("Received SIGINT signal\n");
        QCoreApplication::quit();
        break;
    }
}
