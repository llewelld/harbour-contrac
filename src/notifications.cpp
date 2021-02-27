#include <locale>
#include <notification.h>
#include <QCoreApplication>

#include "appsettings.h"

#include "notifications.h"

// Notification types:
// 1. Download error
// 2. Upload error
// 3. Update status

Notifications *Notifications::instance = nullptr;

Notifications::Notifications(QObject *parent)
    : QObject(parent)
    , m_settings(AppSettings::getInstance())
    , m_uploadNotification(new Notification(this))
    , m_downloadNotification(new Notification(this))
    , m_statusNotification(new Notification(this))
{
    configureNotification(*m_uploadNotification);
    configureNotification(*m_downloadNotification);
    configureNotification(*m_statusNotification);

    //: Upload error notification summary
    //% "Error"
    m_uploadNotification->setSummary(qtTrId("contrac-main_nt_notification-upload-error-summary"));
    //: Upload error notification body
    //% "Diagnosis key upload failed"
    m_uploadNotification->setBody(qtTrId("contrac-main_nt_notification-upload-error-body"));
    m_uploadNotification->setPreviewSummary(qtTrId("contrac-main_nt_notification-upload-error-summary"));
    m_uploadNotification->setPreviewBody(qtTrId("contrac-main_nt_notification-upload-error-body"));

    //: Download error notification summary
    //% "Error"
    m_downloadNotification->setSummary(qtTrId("contrac-main_nt_notification-download-error-summary"));
    //: Download error notification body
    //% "Diagnosis key download failed"
    m_downloadNotification->setBody(qtTrId("contrac-main_nt_notification-download-error-body"));
    m_downloadNotification->setPreviewSummary(qtTrId("contrac-main_nt_notification-download-error-summary"));
    m_downloadNotification->setPreviewBody(qtTrId("contrac-main_nt_notification-download-error-body"));

    //: Risk alert notification summary
    //% "Risk level low"
    m_statusNotification->setSummary(qtTrId("contrac-main_nt_notification-risk-warning-summary"));
    //: Risk alert notification body
    //% "Update completed successfully"
    m_statusNotification->setBody(qtTrId("contrac-main_nt_notification-risk-warning-body"));
    m_statusNotification->setPreviewSummary(qtTrId("contrac-main_nt_notification-risk-warning-summary"));
    m_statusNotification->setPreviewBody(qtTrId("contrac-main_nt_notification-risk-warning-body"));

    QList<quint32> notifyIds = m_settings.notifyIds();
    if (notifyIds.size() == 3) {
        m_uploadNotification->setReplacesId(notifyIds.first());
        notifyIds.pop_front();
        m_downloadNotification->setReplacesId(notifyIds.first());
        notifyIds.pop_front();
        m_statusNotification->setReplacesId(notifyIds.first());
        notifyIds.pop_front();
    }

    connect(&m_settings, &AppSettings::notifyLevelChanged, this, &Notifications::notifyLevelChanged);
}

Notifications::~Notifications()
{
    QList<quint32> notifyIds;
    notifyIds << m_uploadNotification->replacesId();
    notifyIds << m_downloadNotification->replacesId();
    notifyIds << m_statusNotification->replacesId();

    m_settings.setNotifyIds(notifyIds);
}

void Notifications::instantiate(QObject *parent)
{
    if (instance == nullptr) {
        instance = new Notifications(parent);
    }
}

Notifications &Notifications::getInstance()
{
    return *instance;
}

QObject *Notifications::provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return instance;
}

void Notifications::configureNotification(Notification &notification)
{
    //% "Contrac"
    notification.setAppName(qtTrId("contrac-main_nt_notification-app-name"));
    notification.setAppIcon(QStringLiteral("harbour-contrac"));
    notification.setHintValue(QStringLiteral("x-nemo-display-on"), true);
    notification.setHintValue("x-nemo-priority", 100);
    //% "Exposure Notification App"
    notification.setSubText(qtTrId("contrac-main_nt_notification-sub-text"));
}

void Notifications::notifyDownloadError()
{
    if (m_settings.notifyLevel() >= NotifyLevel::Info) {
        m_downloadNotification->publish();
    }
}

void Notifications::notifyUploadError()
{
    if (m_settings.notifyLevel() >= NotifyLevel::Info) {
        m_uploadNotification->publish();
    }
}

void Notifications::notifyUpdateSuccessful()
{
    if (m_settings.notifyLevel() >= NotifyLevel::Info) {
        //% "Exposure level updated"
        m_statusNotification->setSummary(qtTrId("contrac-main_nt_notification-update-summary"));
        //% "Update completed successfully"
        m_statusNotification->setBody(qtTrId("contrac-main_nt_notification-update-body"));
        m_statusNotification->setPreviewSummary(qtTrId("contrac-main_nt_notification-update-summary"));
        m_statusNotification->setPreviewBody(qtTrId("contrac-main_nt_notification-update-body"));

        m_statusNotification->publish();
    }
}

void Notifications::notifyAtRisk(QString const &riskLabel)
{
    if (m_settings.notifyLevel() >= NotifyLevel::Warnings) {
        //% "Risk level: %1"
        m_statusNotification->setSummary(qtTrId("contrac-main_nt_notification-at-risk-body").arg(riskLabel));
        //% "Consult your health service for appropriate action"
        m_statusNotification->setBody(qtTrId("contrac-main_nt_notification-at-risk-summary"));
        m_statusNotification->setPreviewSummary(qtTrId("contrac-main_nt_notification-at-risk-body").arg(riskLabel));
        m_statusNotification->setPreviewBody(qtTrId("contrac-main_nt_notification-at-risk-summary"));

        m_statusNotification->publish();
    }
}

void Notifications::notifyLevelChanged()
{
    switch (m_settings.notifyLevel()) {
    case None:
        m_downloadNotification->close();
        m_uploadNotification->close();
        m_statusNotification->close();
        break;
    case Warnings:
        m_downloadNotification->close();
        m_uploadNotification->close();
        break;
    case Info:
    case All:
    default:
        // Do nothing
        break;
    }
}
