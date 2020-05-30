#ifndef DBUSINTERFACE_H
#define DBUSINTERFACE_H

#include <QObject>
#include <QtDBus>

#define SERVICE_NAME "uk.co.flypig.contrac"
#define SERVICE_PATH "/"

class DBusInterface : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", SERVICE_NAME)
public:
    explicit DBusInterface(QObject *parent = nullptr);
    ~DBusInterface();

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

signals:

public slots:

private:
    QDBusConnection m_connection;
};

#endif // DBUSINTERFACE_H
