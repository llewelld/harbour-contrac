#include "contactmatch.h"

ContactMatch::ContactMatch()
    : m_dtk()
    , m_rpis()
{

}

ContactMatch::ContactMatch(QList<RpiDataItem> const &rpis, DiagnosisKey const *dtk)
    : m_dtk(dtk)
    , m_rpis(rpis)
{
}
