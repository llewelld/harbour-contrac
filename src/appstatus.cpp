#include "appstatus.h"

AppStatus::AppStatus(QObject *parent)
    : QObject(parent)
    , m_status(Disabled)
    , m_statusOfDownload()
    , m_statusOfUpload()
    , m_updating()
    , m_bluetoothBusy()
    , m_bluetoothEnabled()
    , m_atRisk()
    , m_downloadAvailable()
{
}

AppStatus::Status AppStatus::status() const
{
    return m_status;
}

void AppStatus::updateStatus()
{
    Status status = m_status;

    if (m_updating) {
        m_status = Updating;
    }
    else if ((m_statusOfUpload != Upload::StatusIdle) && (m_statusOfUpload != Upload::StatusError)) {
        m_status = Uploading;
    }
    else if ((m_statusOfDownload != Download::StatusIdle) && (m_statusOfDownload != Download::StatusError)) {
        m_status = Downloading;
    }
    else if (m_statusOfUpload == Upload::StatusError) {
        m_status = ErrorUploading;
    }
    else if (m_statusOfDownload == Download::StatusError) {
        m_status = ErrorDownloading;
    }
    else if (m_bluetoothBusy) {
        m_status = BluetoothBusy;
    }
    else if (m_atRisk) {
        m_status = AtRisk;
    }
    else if (m_downloadAvailable) {
        m_status = DailyUpdateRequired;
    }
    else if (m_bluetoothEnabled) {
        m_status = Active;
    }
    else {
        m_status = Disabled;
    }

    if (m_status != status) {
        emit statusChanged();
    }
}

Download::Status AppStatus::statusOfDownload() const
{
    return m_statusOfDownload;
}

Upload::Status AppStatus::statusOfUpload() const
{
    return m_statusOfUpload;
}

bool AppStatus::updating() const
{
    return m_updating;
}

bool AppStatus::bluetoothBusy() const
{
    return m_bluetoothBusy;
}

bool AppStatus::bluetoothEnabled() const
{
    return m_bluetoothEnabled;
}

bool AppStatus::atRisk() const
{
    return m_atRisk;
}

bool AppStatus::downloadAvailable() const
{
    return m_downloadAvailable;
}

void AppStatus::setStatusOfDownload(Download::Status statusOfDownload)
{
    if (m_statusOfDownload != statusOfDownload) {
        m_statusOfDownload = statusOfDownload;
        emit statusOfDownloadChanged();
        updateStatus();
        if (statusOfDownload == Download::StatusError) {
            emit notifyDownloadError();
        }
    }
}

void AppStatus::setStatusOfUpload(Upload::Status statusOfUpload)
{
    if (m_statusOfUpload != statusOfUpload) {
        m_statusOfUpload = statusOfUpload;
        emit statusOfUploadChanged();
        updateStatus();
        if (statusOfUpload == Upload::StatusError) {
            emit notifyUploadError();
        }
    }
}

void AppStatus::setUpdating(bool updating)
{
    if (m_updating != updating) {
        m_updating = updating;
        emit updatingChanged();
        updateStatus();
        if (!updating) {
            if (m_atRisk) {
                emit notifyAtRisk();
            }
            else {
                emit notifyUpdateSuccessful();
            }
        }
    }
}

void AppStatus::setBluetoothBusy(bool bluetoothBusy)
{
    if (m_bluetoothBusy != bluetoothBusy) {
        m_bluetoothBusy = bluetoothBusy;
        emit bluetoothBusyChanged();
        updateStatus();
    }
}

void AppStatus::setBluetoothEnabled(bool bluetoothEnabled)
{
    if (m_bluetoothEnabled != bluetoothEnabled) {
        m_bluetoothEnabled = bluetoothEnabled;
        emit bluetoothEnabledChanged();
        updateStatus();
    }
}

void AppStatus::setAtRisk(bool atRisk)
{
    if (m_atRisk != atRisk) {
        m_atRisk = atRisk;
        emit atRiskChanged();
        updateStatus();
    }
}

void AppStatus::setDownloadAvailable(bool downloadAvailable)
{
    if (m_downloadAvailable != downloadAvailable) {
        m_downloadAvailable = downloadAvailable;
        emit downloadAvailableChanged();
        updateStatus();
    }
}
