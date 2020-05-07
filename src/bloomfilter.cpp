#include "fnv.h"

#include "bloomfilter.h"

namespace {

quint32 hash(quint32 seed, QByteArray const &data)
{
    Fnv32_t hash;

    hash = fnv_32a_buf(& seed, sizeof(seed), FNV1_32A_INIT);
    hash = fnv_32a_buf(data.data(), data.length(), hash);

    return hash;
}

} // Empty namespace

BloomFilter::BloomFilter(quint32 size, quint32 hashes)
{
    clear(size, hashes);
}

void BloomFilter::add(QByteArray const &data)
{
    quint32 seed;

    for (seed = 0; seed < m_hashes; ++seed) {
        quint32 pos = hash(seed, data) % m_size;
        m_filter.setBit(pos, true);
    }
}

bool BloomFilter::test(QByteArray const &data) const
{
    bool hit;
    quint32 seed;

    hit = true;
    for (seed = 0; seed < m_hashes && hit; ++seed) {
        quint32 pos = hash(seed, data) % m_size;
        if (!m_filter.at(pos)) {
            hit = false;
        }
    }

    return hit;
}

void BloomFilter::clear(quint32 size, quint32 hashes)
{
    if (size != 0) {
        m_size = size;
    }
    if (hashes != 0) {
        m_hashes = hashes;
    }

    m_filter.clear();
    m_filter.fill(0, m_size);
}

QByteArray const BloomFilter::BloomFilter::getFilter() const
{
    QByteArray bytes;
    unsigned int pos;

    for (pos = 0; pos < m_size / 8; ++pos) {
        char byte = 0;
        for (int bit = 0; bit < 8; ++bit) {
            byte |= (m_filter.at((pos * 8) + bit) ? 1 : 0) << bit;
        }
        bytes.append(byte);
    }
    return bytes;
}

void BloomFilter::setFilter(QByteArray const &filter, quint32 hashes)
{
    unsigned int pos;

    m_size = filter.size() * 8;
    m_hashes = hashes;

    m_filter.clear();
    m_filter.fill(0, m_size);
    for (pos = 0; pos < m_size; ++pos) {
        m_filter.setBit(pos, filter.at(pos / 8) & (1 << (pos % 8)));
    }
}

quint32 BloomFilter::getSize() const
{
    return m_size;
}

quint32 BloomFilter::getHahes() const
{
    return m_hashes;
}
