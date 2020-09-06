#ifndef EXPOSURECONFIGURATION_H
#define EXPOSURECONFIGURATION_H

#include <QObject>
#include <QDBusArgument>

class ExposureConfiguration : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qint32 minimumRiskScore READ minimumRiskScore WRITE setMinimumRiskScore NOTIFY minimumRiskScoreChanged)
    Q_PROPERTY(QList<qint32> attenuationScores READ attenuationScores WRITE setAttenuationScores NOTIFY attenuationScoresChanged)
    Q_PROPERTY(QList<qint32> daysSinceLastExposureScores READ daysSinceLastExposureScores WRITE setDaysSinceLastExposureScores NOTIFY daysSinceLastExposureScoresChanged)
    Q_PROPERTY(QList<qint32> durationScores READ durationScores WRITE setDurationScores NOTIFY durationScoresChanged)
    Q_PROPERTY(QList<qint32> transmissionRiskScores READ transmissionRiskScores WRITE setTransmissionRiskScores NOTIFY transmissionRiskScoresChanged)

    Q_PROPERTY(double attenuationWeight READ attenuationWeight WRITE setAttenuationWeight NOTIFY attenuationWeightChanged)
    Q_PROPERTY(double daysSinceLastExposureWeight READ daysSinceLastExposureWeight WRITE setDaysSinceLastExposureWeight NOTIFY daysSinceLastExposureWeightChanged)
    Q_PROPERTY(double durationWeight READ durationWeight WRITE setDurationWeight NOTIFY durationWeightChanged)
    Q_PROPERTY(double transmissionRiskWeight READ transmissionRiskWeight WRITE setTransmissionRiskWeight NOTIFY transmissionRiskWeightChanged)

    Q_PROPERTY(QList<qint32> durationAtAttenuationThresholds READ durationAtAttenuationThresholds WRITE setDurationAtAttenuationThresholds NOTIFY durationAtAttenuationThresholdsChanged)
public:
    explicit ExposureConfiguration(QObject *parent = nullptr);
    ExposureConfiguration(ExposureConfiguration const &exposureConfiguration);
    ExposureConfiguration& operator=( const ExposureConfiguration &other);

    qint32 minimumRiskScore() const;
    QList<qint32> attenuationScores() const;
    QList<qint32> daysSinceLastExposureScores() const;
    QList<qint32> durationScores() const;
    QList<qint32> transmissionRiskScores() const;
    double attenuationWeight() const;
    double daysSinceLastExposureWeight() const;
    double durationWeight() const;
    double transmissionRiskWeight() const;
    QList<qint32> durationAtAttenuationThresholds() const;

    void setMinimumRiskScore(qint32 minimumRiskScore);
    void setAttenuationScores(QList<qint32> attenuationScores);
    void setDaysSinceLastExposureScores(QList<qint32> daysSinceLastExposureScores);
    void setDurationScores(QList<qint32> durationScores);
    void setTransmissionRiskScores(QList<qint32> transmissionRiskScores);
    void setAttenuationWeight(double attenuationWeight);
    void setDaysSinceLastExposureWeight(double daysSinceLastExposureWeight);
    void setDurationWeight(double durationWeight);
    void setTransmissionRiskWeight(double transmissionRiskWeight);
    void setDurationAtAttenuationThresholds(QList<qint32> durationAtAttenuationThresholds);

signals:
    void minimumRiskScoreChanged();
    void attenuationScoresChanged();
    void daysSinceLastExposureScoresChanged();
    void durationScoresChanged();
    void transmissionRiskScoresChanged();
    void attenuationWeightChanged();
    void daysSinceLastExposureWeightChanged();
    void durationWeightChanged();
    void transmissionRiskWeightChanged();
    void durationAtAttenuationThresholdsChanged();

private:
    qint32 m_minimumRiskScore;
    QList<qint32> m_attenuationScores;
    QList<qint32> m_daysSinceLastExposureScores;
    QList<qint32> m_durationScores;
    QList<qint32> m_transmissionRiskScores;

    double m_attenuationWeight;
    double m_daysSinceLastExposureWeight;
    double m_durationWeight;
    double m_transmissionRiskWeight;
    QList<qint32> m_durationAtAttenuationThresholds;
};

QDBusArgument &operator<<(QDBusArgument &argument, const ExposureConfiguration &exposureConfiguration);
QDBusArgument const &operator>>(const QDBusArgument &argument, ExposureConfiguration &exposureConfiguration);

Q_DECLARE_METATYPE(ExposureConfiguration)

#endif // EXPOSURECONFIGURATION_H
