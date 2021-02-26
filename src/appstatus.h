#ifndef APPSTATUS_H
#define APPSTATUS_H

#include <QObject>

#include "download.h"
#include "upload.h"

class AppStatus : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)

    Q_PROPERTY(Download::Status statusOfDownload READ statusOfDownload WRITE setStatusOfDownload NOTIFY statusOfDownloadChanged)
    Q_PROPERTY(Upload::Status statusOfUpload READ statusOfUpload WRITE setStatusOfUpload NOTIFY statusOfUploadChanged)
    Q_PROPERTY(bool updating READ updating WRITE setUpdating NOTIFY updatingChanged)
    Q_PROPERTY(bool bluetoothBusy READ bluetoothBusy WRITE setBluetoothBusy NOTIFY bluetoothBusyChanged)
    Q_PROPERTY(bool bluetoothEnabled READ bluetoothEnabled WRITE setBluetoothEnabled NOTIFY bluetoothEnabledChanged)
    Q_PROPERTY(bool atRisk READ atRisk WRITE setAtRisk NOTIFY atRiskChanged)
    Q_PROPERTY(bool downloadAvailable READ downloadAvailable WRITE setDownloadAvailable NOTIFY downloadAvailableChanged)

public:
    explicit AppStatus(QObject *parent = nullptr);

    enum Status
    {
        Updating,
        Uploading,
        Downloading,
        ErrorUploading,
        ErrorDownloading,
        BluetoothBusy,
        AtRisk,
        DailyUpdateRequired,
        Active,
        Disabled
    };
    Q_ENUM(Status)

    Status status() const;

    Download::Status statusOfDownload() const;
    Upload::Status statusOfUpload() const;
    bool updating() const;
    bool bluetoothBusy() const;
    bool bluetoothEnabled() const;
    bool atRisk() const;
    bool downloadAvailable() const;

    void setStatusOfDownload(Download::Status statusOfDownload);
    void setStatusOfUpload(Upload::Status statusOfUpload);
    void setUpdating(bool updating);
    void setBluetoothBusy(bool bluetoothBusy);
    void setBluetoothEnabled(bool bluetoothEnabled);
    void setAtRisk(bool atRisk);
    void setDownloadAvailable(bool downloadAvailable);

signals:
    void statusChanged();
    void statusOfDownloadChanged();
    void statusOfUploadChanged();
    void updatingChanged();
    void bluetoothBusyChanged();
    void bluetoothEnabledChanged();
    void atRiskChanged();
    void downloadAvailableChanged();

    // Notifications
    void notifyUpdateSuccessful();
    void notifyDownloadError();
    void notifyUploadError();
    void notifyAtRisk();

private:
    void updateStatus();

private:
    Status m_status;
    Download::Status m_statusOfDownload;
    Upload::Status m_statusOfUpload;
    bool m_updating;
    bool m_bluetoothBusy;
    bool m_bluetoothEnabled;
    bool m_atRisk;
    bool m_downloadAvailable;
};

#endif // APPSTATUS_H
