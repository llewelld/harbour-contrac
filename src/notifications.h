#ifndef NOTIFICATIONS_H
#define NOTIFICATIONS_H

#include <QDebug>
#include <QObject>

class Notification;
class AppSettings;

class QQmlEngine;
class QJSEngine;

class Notifications : public QObject
{
    Q_OBJECT

public:
    explicit Notifications(QObject *parent = nullptr);
    ~Notifications();

    static void instantiate(QObject *parent = nullptr);
    static Notifications &getInstance();
    static QObject *provider(QQmlEngine *engine, QJSEngine *scriptEngine);

    enum NotifyLevel
    {
        None,
        Warnings,
        Info,
        All
    };
    Q_ENUM(NotifyLevel)

public slots:
    void notifyDownloadError();
    void notifyUploadError();
    void notifyUpdateSuccessful();
    void notifyAtRisk(QString const &riskLabel);

private:
    void configureNotification(Notification &notification);

private slots:
    void notifyLevelChanged();

private:
    static Notifications *instance;

    AppSettings &m_settings;
    Notification *m_uploadNotification;
    Notification *m_downloadNotification;
    Notification *m_statusNotification;
};

#endif // NOTIFICATIONS_H
