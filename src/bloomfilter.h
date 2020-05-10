#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H

#include <QtGlobal>
#include <QBitArray>

#define BLOOM_DEFAULT_SIZE (4)
#define BLOOM_DEFAULT_HASHES (1)

// Optimisation
// k - hashes
// m - bits in filter
// n - number of elements to be inserted.
//
// optimal k = (m / n) ln(2)
// false positive rate r = (1 - e^{-kn/m})^k

/*
def fpr(n, m):
    # k = (m/n)ln(2)
    # r = (1-e^{-kn/m})^k
    k = round((m / n) * math.log(2.0))
    r = (1.0 - math.exp(-k * n / m))**k
    print("k = {}".format(k))
    print("r = {}".format(r))
*/

// See: https://llimllib.github.io/bloomfilter-tutorial/

class BloomFilter
{
public:
    BloomFilter(quint32 day = 0, quint32 size = BLOOM_DEFAULT_SIZE, quint32 hashes = BLOOM_DEFAULT_HASHES);

    void add(QByteArray const &data);
    bool test(QByteArray const &data) const;
    void clear(quint32 size = 0, quint32 hashes = 0);
    QByteArray const getFilter() const;
    void setFilter(QByteArray const &filter, quint32 hashes);
    quint32 getSize() const;
    quint32 getHahes() const;
    bool load(quint32 day);
    bool save() const;
    void setDay(quint32 day);
    quint32 day() const;

private:
    quint32 m_size;
    quint32 m_hashes;
    quint32 m_day;
    QBitArray m_filter;
};

#endif // BLOOMFILTER_H
