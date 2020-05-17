#ifndef EXPOSUREINFORMATION_H
#define EXPOSUREINFORMATION_H

#include <QObject>

class ExposureInformation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint64 dateMillisSinceEpoch READ dateMillisSinceEpoch WRITE setDateMillisSinceEpoch NOTIFY dateMillisSinceEpochChanged)
    Q_PROPERTY(quint32 durationMinutes READ durationMinutes WRITE setDurationMinutes NOTIFY durationMinutesChanged)
    Q_PROPERTY(quint32 attenuationValue READ attenuationValue WRITE setAttenuationValue NOTIFY attenuationValueChanged)
    Q_PROPERTY(quint32 transmissionRiskLevel READ transmissionRiskLevel WRITE setTransmissionRiskLevel NOTIFY transmissionRiskLevelChanged)
    Q_PROPERTY(quint32 totalRiskScore READ totalRiskScore WRITE setTotalRiskScore NOTIFY totalRiskScoreChanged)
    Q_PROPERTY(QList<quint32> attenuationDurations READ attenuationDurations WRITE setAttenuationDurations NOTIFY attenuationDurationsChanged)
public:
    explicit ExposureInformation(QObject *parent = nullptr);
    explicit ExposureInformation(ExposureInformation const &exposureInformation);

    quint64 dateMillisSinceEpoch() const;
    quint32 durationMinutes() const;
    quint32 attenuationValue() const;
    quint32 transmissionRiskLevel() const;
    quint32 totalRiskScore() const;
    QList<quint32> attenuationDurations() const;

    void setDateMillisSinceEpoch(quint64 dateMillisSinceEpoch);
    void setDurationMinutes(quint32 durationMinutes);
    void setAttenuationValue(quint32 attenuationValue);
    void setTransmissionRiskLevel(quint32 transmissionRiskLevel);
    void setTotalRiskScore(quint32 totalRiskScore);
    void setAttenuationDurations(QList<quint32> attenuationDurations);

signals:
    void dateMillisSinceEpochChanged();
    void durationMinutesChanged();
    void attenuationValueChanged();
    void transmissionRiskLevelChanged();
    void totalRiskScoreChanged();
    void attenuationDurationsChanged();

public slots:

private:
    quint64 m_dateMillisSinceEpoch;
    quint32 m_durationMinutes;
    quint32 m_attenuationValue;
    quint32 m_transmissionRiskLevel;
    quint32 m_totalRiskScore;
    QList<quint32> m_attenuationDurations;
};

#endif // EXPOSUREINFORMATION_H
