#ifndef EXPOSURECONFIGURATION_H
#define EXPOSURECONFIGURATION_H

#include <QObject>

class ExposureConfiguration : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint8 minimumRiskScore MEMBER m_minimumRiskScore NOTIFY minimumRiskScoreChanged)

    Q_PROPERTY(QList<quint32> attenuationScores READ attenuationScores WRITE setAttenuationScores NOTIFY attenuationScoresChanged)
    Q_PROPERTY(QList<quint32> daysSinceLastExposureScores READ daysSinceLastExposureScores WRITE setDaysSinceLastExposureScores NOTIFY daysSinceLastExposureScoresChanged)
    Q_PROPERTY(QList<quint32> durationScores READ durationScores WRITE setDurationScores NOTIFY durationScoresChanged)
    Q_PROPERTY(QList<quint32> transmissionRiskScores READ transmissionRiskScores WRITE setTransmissionRiskScores NOTIFY transmissionRiskScoresChanged)

    Q_PROPERTY(double attenuationWeight READ attenuationWeight WRITE setAttenuationWeight NOTIFY attenuationWeightChanged)
    Q_PROPERTY(double daysSinceLastExposureWeight READ daysSinceLastExposureWeight WRITE setDaysSinceLastExposureWeight NOTIFY daysSinceLastExposureWeightChanged)
    Q_PROPERTY(double durationWeight READ durationWeight WRITE setDurationWeight NOTIFY durationWeightChanged)
    Q_PROPERTY(double transmissionRiskWeight READ transmissionRiskWeight WRITE setTransmissionRiskWeight NOTIFY transmissionRiskWeightChanged)

public:
    explicit ExposureConfiguration(QObject *parent = nullptr);

    QList<quint32> attenuationScores() const;
    QList<quint32> daysSinceLastExposureScores() const;
    QList<quint32> durationScores() const;
    QList<quint32> transmissionRiskScores() const;
    double attenuationWeight() const;
    double daysSinceLastExposureWeight() const;
    double durationWeight() const;
    double transmissionRiskWeight() const;

    void setAttenuationScores(QList<quint32> attenuationScores);
    void setDaysSinceLastExposureScores(QList<quint32> daysSinceLastExposureScores);
    void setDurationScores(QList<quint32> durationScores);
    void setTransmissionRiskScores(QList<quint32> transmissionRiskScores);
    void setAttenuationWeight(double attenuationWeight);
    void setDaysSinceLastExposureWeight(double daysSinceLastExposureWeight);
    void setDurationWeight(double durationWeight);
    void setTransmissionRiskWeight(double transmissionRiskWeight);
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

private:
    quint8 m_minimumRiskScore;
    QList<quint32> m_attenuationScores;
    QList<quint32> m_daysSinceLastExposureScores;
    QList<quint32> m_durationScores;
    QList<quint32> m_transmissionRiskScores;

    double m_attenuationWeight;
    double m_daysSinceLastExposureWeight;
    double m_durationWeight;
    double m_transmissionRiskWeight;
};

#endif // EXPOSURECONFIGURATION_H
