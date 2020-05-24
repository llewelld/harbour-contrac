#ifndef CONTACTMATCH_H
#define CONTACTMATCH_H

#include <QByteArray>

#include "diagnosiskey.h"
#include "rpidataitem.h"

class ContactMatch
{
public:
    ContactMatch();
    ContactMatch(QList<RpiDataItem> const &rpis, DiagnosisKey const *dtk);

public:
    DiagnosisKey const *m_dtk;
    QList<RpiDataItem> m_rpis;
};

#endif // CONTACTMATCH_H
