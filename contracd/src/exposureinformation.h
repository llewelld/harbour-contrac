#ifndef EXPOSUREINFORMATION_H
#define EXPOSUREINFORMATION_H

#include <QObject>
#include <QDBusArgument>

class ExposureInformation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint64 dateMillisSinceEpoch READ dateMillisSinceEpoch WRITE setDateMillisSinceEpoch NOTIFY dateMillisSinceEpochChanged)
    Q_PROPERTY(qint32 durationMinutes READ durationMinutes WRITE setDurationMinutes NOTIFY durationMinutesChanged)
    Q_PROPERTY(qint32 attenuationValue READ attenuationValue WRITE setAttenuationValue NOTIFY attenuationValueChanged)
    Q_PROPERTY(qint32 transmissionRiskLevel READ transmissionRiskLevel WRITE setTransmissionRiskLevel NOTIFY transmissionRiskLevelChanged)
    Q_PROPERTY(qint32 totalRiskScore READ totalRiskScore WRITE setTotalRiskScore NOTIFY totalRiskScoreChanged)
    Q_PROPERTY(QList<qint32> attenuationDurations READ attenuationDurations WRITE setAttenuationDurations NOTIFY attenuationDurationsChanged)
public:
    explicit ExposureInformation(QObject *parent = nullptr);
    ExposureInformation(ExposureInformation const &exposureInformation);
    ExposureInformation& operator=( const ExposureInformation &other);

    quint64 dateMillisSinceEpoch() const;
    qint32 durationMinutes() const;
    qint32 attenuationValue() const;
    qint32 transmissionRiskLevel() const;
    qint32 totalRiskScore() const;
    QList<qint32> attenuationDurations() const;

    void setDateMillisSinceEpoch(quint64 dateMillisSinceEpoch);
    void setDurationMinutes(qint32 durationMinutes);
    void setAttenuationValue(qint32 attenuationValue);
    void setTransmissionRiskLevel(qint32 transmissionRiskLevel);
    void setTotalRiskScore(qint32 totalRiskScore);
    void setAttenuationDurations(QList<qint32> attenuationDurations);

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
    qint32 m_durationMinutes;
    qint32 m_attenuationValue;
    qint32 m_transmissionRiskLevel;
    qint32 m_totalRiskScore;
    QList<qint32> m_attenuationDurations;
};

QDBusArgument &operator<<(QDBusArgument &argument, const ExposureInformation &exposureInformation);
QDBusArgument const &operator>>(const QDBusArgument &argument, ExposureInformation &exposureInformation);

Q_DECLARE_METATYPE(ExposureInformation)

QDBusArgument &operator<<(QDBusArgument &argument, const QList<ExposureInformation> &exposureInformationList);
QDBusArgument const &operator>>(const QDBusArgument &argument, QList<ExposureInformation> &exposureInformationList);

Q_DECLARE_METATYPE(QList<ExposureInformation>)

#endif // EXPOSUREINFORMATION_H
