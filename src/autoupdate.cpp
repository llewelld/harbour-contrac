#include <QDateTime>
#include <QDebug>

#include <backgroundactivity.h>

#include "appsettings.h"
#include "autoupdate.h"

AutoUpdate::AutoUpdate(QObject *parent)
    : QObject(parent)
    , m_backgroundActivity(new BackgroundActivity(this))
    , m_running(false)
    , m_settings(AppSettings::getInstance())
{
    connect(m_backgroundActivity, &BackgroundActivity::stateChanged, this, &AutoUpdate::stateChanged);

    connect(&m_settings, &AppSettings::autoUpdateChanged, this, &AutoUpdate::autoUpdateSettingChanged);
    connect(&m_settings, &AppSettings::autoUpdateTimeChanged, this, &AutoUpdate::autoUpdateTimeSettingChanged);

    configureNextUpdate();
}

AutoUpdate::~AutoUpdate()
{
    qDebug() << "Stopping all auto update triggers";
    m_running = false;
    m_backgroundActivity->stop();

    emit runningChanged();
}

bool AutoUpdate::running() const
{
    return m_running;
}

void AutoUpdate::setRunning(bool running)
{
    if (m_running != running) {
        m_running = running;
        emit runningChanged();
    }
}

bool AutoUpdate::autoUpdate() const
{
    return m_settings.autoUpdate();
}

void AutoUpdate::setAutoUpdate(bool autoUpdate)
{
    m_settings.setAutoUpdate(autoUpdate);

    if (!m_running) {
        if (autoUpdate) {
            qDebug() << "Enabling auto update for:" << m_settings.autoUpdateTime();
            m_backgroundActivity->wait();
        }
        else {
            qDebug() << "Disabling auto update";
            m_backgroundActivity->stop();
        }
    }
}

qint32 AutoUpdate::updateTime() const
{
    return m_settings.autoUpdateTime();
}

void AutoUpdate::setUpdateTime(qint32 time)
{
    m_settings.setAutoUpdateTime(time);
}

void AutoUpdate::stateChanged()
{
    if (m_backgroundActivity->isRunning()) {
        m_running = true;
        emit runningChanged();
        qDebug() << "Auto update started";
        emit startUpdate();
    }
}

void AutoUpdate::updateComplete()
{
    if (m_running) {
        qDebug() << "Auto update complete";
        m_running = false;

        if (autoUpdate()) {
            configureNextUpdate();
        }
        else {
            m_backgroundActivity->stop();
        }

        emit runningChanged();
    }
}

void AutoUpdate::configureNextUpdate()
{
    qint32 const autoUpdateTime = m_settings.autoUpdateTime();
    qDebug() << "Configuring auto update time to:" << autoUpdateTime;

    qDebug() << "Current time:" << (QTime::currentTime().msecsSinceStartOfDay() / 1000);

    int delay = (60 * 60 * 24) + m_settings.autoUpdateTime();
    delay -= (QTime::currentTime().msecsSinceStartOfDay() / 1000);
    delay = delay % (60 * 60 * 24);
    if (delay < 60) {
        delay += 60 * 60 * 24;
    }

    // To the nearest quarter of an hour
    m_backgroundActivity->setWakeupRange(delay, delay + (60 * 5));

    if (autoUpdate()) {
        qDebug() << "AutoUpdate active, will trigger in" << delay << "seconds";
        if (!m_running) {
            m_backgroundActivity->wait();
        }
    }
    else {
        qDebug() << "AutoUpdate inactive, would trigger in" << delay << "seconds";
    }
}

void AutoUpdate::autoUpdateSettingChanged()
{
    configureNextUpdate();

    emit autoUpdateChanged();
}

void AutoUpdate::autoUpdateTimeSettingChanged()
{
    configureNextUpdate();

    emit updateTimeChanged();
}
