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

#include "contracd.h"

static void signal_handler(int sig);

int main(int argc, char *argv[])
{
    int result;

    QCoreApplication *app = new QCoreApplication(argc, argv);
    QCoreApplication::setOrganizationDomain("www.flypig.co.uk");
    QCoreApplication::setApplicationName("contracd");

    umask(0);

    result = chdir("/tmp");

    if (result >= 0) {
        signal(SIGHUP, signal_handler);
        signal(SIGTERM, signal_handler);

        setlinebuf(stdout);
        setlinebuf(stderr);
    }

    if (result >= 0) {
        result = app->exec();
    }

    qDebug() << "Execution finished: " << result;
    delete app;
    qDebug() << "Deleted app";

    return result;
}

static void signal_handler(int sig)
{
    switch (sig) {
    case SIGHUP:
        printf("Received SIGHUP signal");
        break;
    case SIGTERM:
        printf("Received SIGTERM signal");
        QCoreApplication::quit();
        break;
    }
}
