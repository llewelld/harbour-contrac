#ifndef EXPOSURENOTIFICATION_P_H
#define EXPOSURENOTIFICATION_P_H

#include <QObject>
#include <QTimer>

#include "exposureinformation.h"
#include "contrac.h"
#include "exposurenotification.h"

namespace diagnosis {
class TemporaryExposureKeyExport;
} // namespace diagnosis

class ExposureNotificationPrivate : public QObject
{
    Q_OBJECT
public:
    explicit ExposureNotificationPrivate(ExposureNotification *q);
    ~ExposureNotificationPrivate();

    static bool loadDiagnosisKeys(QString const &keyFile, diagnosis::TemporaryExposureKeyExport * keyExport);
    QList<ExposureInformation> aggregateExposureData(quint32 dayNumber, ExposureConfiguration const &configuration, QList<ContactMatch> matches);
    static quint32 calculateRiskScore(ExposureConfiguration const &configuration, qint32 transmissionRisk, qint32 duration, qint32 days_ago, qint32 attenuationValue);

private:
    ExposureNotification *q_ptr;

    Q_DECLARE_PUBLIC(ExposureNotification)

public slots:
    void scanChanged();

public:
    QMap<QString, QList<ExposureInformation>> m_exposures;
    ExposureNotification::Status m_status;
    Contrac *m_contrac;
    BleScanner *m_scanner;
    Controller *m_controller;
    ContactStorage *m_contacts;
    QTimer m_intervalUpdate;
};

#endif // EXPOSURENOTIFICATION_P_H
