#ifndef METADATA_H
#define METADATA_H

#include <QObject>

#define AEM_SIZE (4u)
#define TX_POWER_UNKNOWN (0)

class Metadata
{
public:
    explicit Metadata();
    explicit Metadata(QByteArray const &dtk, QByteArray const &rpi, qint8 txPower);
    explicit Metadata(QByteArray const &aem);

    void setDtk(QByteArray const &dtk);
    void setRpi(QByteArray const &rpi);
    void setTxPower(qint8 txPower);
    void setEncryptedMetadata(QByteArray const &aem);

    bool valid() const;
    qint8 txPower();
    QByteArray metadata();
    QByteArray encryptedMetadata();

private:
    bool encryptMetadata();
    bool decryptMetadata();

public:
    QByteArray m_dtk;
    QByteArray m_rpi;
    qint8 m_txPower;
    QByteArray m_aem;
    bool m_needsEncrypting;
    bool m_needsDecrypting;
    bool m_valid;
};

#endif // METADATA_H
