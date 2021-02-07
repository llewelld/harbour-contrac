#include <QDataStream>

#include "applicationConfiguration.pb.h"

#include "riskscoreclass.h"

RiskScoreClass::RiskScoreClass(QObject *parent)
    : QObject(parent)
    , m_label()
    , m_min(0)
    , m_max(0)
{
}

RiskScoreClass::RiskScoreClass(RiskScoreClass const &riskScoreClass)
    : QObject(riskScoreClass.parent())
    , m_label(riskScoreClass.m_label)
    , m_min(riskScoreClass.m_min)
    , m_max(riskScoreClass.m_max)
{
}

RiskScoreClass::RiskScoreClass(::diagnosis::RiskScoreClass const &riskScoreClass, QObject *parent)
    : QObject(parent)
{
    m_label = QString::fromStdString(riskScoreClass.label());
    m_min = riskScoreClass.min();
    m_max = riskScoreClass.max();
}

RiskScoreClass &RiskScoreClass::operator=(const RiskScoreClass &other)
{
    if (this != &other) {
        m_label = other.m_label;
        m_min = other.m_min;
        m_max = other.m_max;
    }

    return *this;
}

bool RiskScoreClass::operator==(const RiskScoreClass &other) const
{
    bool same;
    same = (m_label == other.m_label) && (m_min == other.m_min) && (m_max == other.m_max);

    return same;
}

bool RiskScoreClass::operator!=(const RiskScoreClass &other) const
{
    return !(*this == other);
}

bool RiskScoreClass::operator==(const ::diagnosis::RiskScoreClass &other) const
{
    bool same;
    same = (m_label == QString::fromStdString(other.label())) && (m_min == other.min()) && (m_max == other.max());

    return same;
}

bool RiskScoreClass::operator!=(const ::diagnosis::RiskScoreClass &other) const
{
    return !(*this == other);
}

QDataStream &operator<<(QDataStream &out, const RiskScoreClass &riskScoreClass)
{
    out << riskScoreClass.label();
    out << riskScoreClass.min();
    out << riskScoreClass.max();

    return out;
}

QDataStream &operator>>(QDataStream &in, RiskScoreClass &riskScoreClass)
{
    QString valueString;
    qint32 valueSignedInt;

    in >> valueString;
    riskScoreClass.setLabel(valueString);
    in >> valueSignedInt;
    riskScoreClass.setMax(valueSignedInt);
    in >> valueSignedInt;
    riskScoreClass.setMax(valueSignedInt);

    return in;
}

QString RiskScoreClass::label() const
{
    return m_label;
}

void RiskScoreClass::setLabel(QString label)
{
    if (m_label != label) {
        m_label = label;
        emit labelChanged();
    }
}

qint32 RiskScoreClass::min() const
{
    return m_min;
}

void RiskScoreClass::setMin(qint32 min)
{
    if (m_min != min) {
        m_min = min;
        emit minChanged();
    }
}

qint32 RiskScoreClass::max() const
{
    return m_max;
}

void RiskScoreClass::setMax(qint32 max)
{
    if (m_max != max) {
        m_max = max;
        emit maxChanged();
    }
}
