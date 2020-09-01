#include <QDebug>
#include <openssl/crypto.h>
#include <openssl/evp.h>

#include "hkdfsha256.h"

#include "metadata.h"

#define AEMK_SIZE (16u)
#define AEMK_INFO "EN-AEMK"
#define AEM_VERSION (0b01000000)

Metadata::Metadata()
    : m_dtk()
    , m_rpi()
    , m_txPower(TX_POWER_UNKNOWN)
    , m_aem()
    , m_needsEncrypting(true)
    , m_needsDecrypting(false)
    , m_valid(false)
{

}

Metadata::Metadata(QByteArray const &dtk, QByteArray const &rpi, qint8 txPower)
    : m_dtk(dtk)
    , m_rpi(rpi)
    , m_txPower(txPower)
    , m_aem()
    , m_needsEncrypting(true)
    , m_needsDecrypting(false)
    , m_valid(true)
{
}

Metadata::Metadata(QByteArray const &aem)
    : m_dtk()
    , m_rpi()
    , m_txPower()
    , m_aem(aem)
    , m_needsEncrypting(false)
    , m_needsDecrypting(true)
    , m_valid(true)
{
}

void Metadata::setDtk(QByteArray const &dtk)
{
    if (m_dtk != dtk) {
        m_dtk = dtk;
        m_needsEncrypting = true;
    }
}

void Metadata::setRpi(QByteArray const &rpi)
{
    if (m_rpi != rpi) {
        m_rpi = rpi;
        m_needsEncrypting = true;
    }
}

void Metadata::setTxPower(qint8 txPower)
{
    if (m_txPower != txPower) {
        if (m_needsDecrypting) {
            decryptMetadata();
        }
        m_txPower = txPower;
        m_needsEncrypting = true;
    }
}

void Metadata::setEncryptedMetadata(QByteArray const &aem)
{
    if (m_aem != aem) {
        m_aem = aem;
        m_needsDecrypting = true;
    }
}

qint8 Metadata::txPower()
{
    m_valid = true;
    if (m_needsDecrypting) {
        m_valid = decryptMetadata();
    }

    return m_valid ? m_txPower : TX_POWER_UNKNOWN;
}

QByteArray Metadata::metadata()
{
    QByteArray metadata;

    m_valid = true;
    if (m_needsDecrypting) {
        m_valid = decryptMetadata();
    }
    if (m_valid) {
        metadata.fill(0, 4);
        metadata[0] = AEM_VERSION;
        metadata[1] = static_cast<quint8>(m_txPower);
    }

    return metadata;
}

QByteArray Metadata::encryptedMetadata()
{
    m_valid = true;
    if (m_needsEncrypting) {
        m_valid = encryptMetadata();
    }

    return m_valid ? m_aem : QByteArray();
}

bool Metadata::valid() const
{
    return m_valid;
}


bool Metadata::encryptMetadata()
{
    QByteArray const metadata = this->metadata();
    int result;
    unsigned char aemk[AEMK_SIZE];
    unsigned int out_length = 0;
    unsigned char const * dtk_data;
    unsigned char const * rpi_data;
    unsigned char salt[4];
    unsigned char encode[sizeof(AEMK_INFO)];
    int aem_length = 0;
    unsigned char aem_data[AEM_SIZE];
    EVP_CIPHER_CTX *ctx;

    m_aem.clear();

    // AEMK_i = HKDF(tek_i, NULL, UTF8("EN-AEMK"), 16)
    // FORMAT: Output = HKDF(Key, Salt, Info, OutputLength )
    dtk_data = reinterpret_cast<unsigned char const *>(m_dtk.data());
    out_length = AEMK_SIZE;
    memcpy(encode, AEMK_INFO, sizeof(AEMK_INFO));
    result = HKDF(aemk, out_length, EVP_sha256(), dtk_data, static_cast<size_t>(m_dtk.size()), salt, 0, encode, sizeof(encode));

    // Associated Encrypted Metadata_{i, j} = AES_128-CTR(AEMK_i, RPI_{i, j}, Metadata)
    // FORMAT: Ciphertext = AES_128-CTR(Key, IV, Data)
    ctx = EVP_CIPHER_CTX_new();
    if (result > 0) {
        rpi_data = reinterpret_cast<unsigned char const *>(m_rpi.data());
        unsigned char const *key = aemk;
        unsigned char const *iv = rpi_data;
        unsigned char const *data = reinterpret_cast<unsigned char const *>(metadata.data());

        EVP_EncryptInit_ex(ctx, EVP_aes_128_ctr(), nullptr, key, iv);

        aem_length = AEM_SIZE;
        result = EVP_EncryptUpdate(ctx, aem_data, &aem_length, data, metadata.length());
    }
    else {
        qDebug() << "Error generating metadata key";
    }

    if ((result > 0) && (aem_length == AEM_SIZE)) {
        int final_length = 0;
        result = EVP_EncryptFinal_ex(ctx, aem_data + aem_length, &final_length);
        aem_length += final_length;
    }
    else {
        qDebug() << "Error encrypting metadata";
    }

    if ((result > 0) && (aem_length == AEM_SIZE)) {
        m_aem = QByteArray(reinterpret_cast<char const *>(aem_data), aem_length);
        m_needsEncrypting = false;
    }
    else {
        qDebug() << "Error finalising metadata encryption";
    }

    EVP_CIPHER_CTX_free(ctx);

    return result;
}

bool Metadata::decryptMetadata()
{
    QByteArray metadata;
    int result;
    unsigned char aemk[AEMK_SIZE];
    unsigned int out_length = 0;
    unsigned char const * dtk_data;
    unsigned char const * rpi_data;
    unsigned char salt[4];
    unsigned char encode[sizeof(AEMK_INFO)];
    int data_length = 0;
    unsigned char data[AEM_SIZE];
    EVP_CIPHER_CTX *ctx;

    // AEMK_i = HKDF(tek_i, NULL, UTF8("EN-AEMK"), 16)
    // FORMAT: Output = HKDF(Key, Salt, Info, OutputLength )
    dtk_data = reinterpret_cast<unsigned char const *>(m_dtk.data());
    out_length = AEMK_SIZE;
    memcpy(encode, AEMK_INFO, sizeof(AEMK_INFO));
    result = HKDF(aemk, out_length, EVP_sha256(), dtk_data, static_cast<size_t>(m_dtk.size()), salt, 0, encode, sizeof(encode));

    // Associated Encrypted Metadata_{i, j} = AES_128-CTR(AEMK_i, RPI_{i, j}, Metadata)
    // FORMAT: Ciphertext = AES_128-CTR(Key, IV, Data)
    ctx = EVP_CIPHER_CTX_new();
    if (result > 0) {
        rpi_data = reinterpret_cast<unsigned char const *>(m_rpi.data());
        unsigned char const *key = aemk;
        unsigned char const *iv = rpi_data;
        unsigned char const *aem_data = reinterpret_cast<unsigned char const *>(m_aem.data());

        EVP_DecryptInit_ex(ctx, EVP_aes_128_ctr(), nullptr, key, iv);

        data_length = AEM_SIZE;
        result = EVP_DecryptUpdate(ctx, data, &data_length, aem_data, m_aem.length());
    }
    else {
        qDebug() << "Error generating metadata key";
    }

    if (result > 0) {
        int final_length = 0;
        result = EVP_DecryptFinal_ex(ctx, data + data_length, &final_length);
        data_length += final_length;
    }
    else {
        qDebug() << "Error decrypting aem";
    }

    if (result > 0) {
        result = (data_length == AEM_SIZE) ? 1 : 0;
    }
    else {
        qDebug() << "Error finalising metadata decryption";
    }

    if (result > 0) {
        // Check validity
        result = ((data[0] == AEM_VERSION) && (data[2] == 0) && (data[3] == 0)) ? 1 : 0;
    }
    else {
        qDebug() << "Decrypted data of incorrect length";
    }

    if (result) {
        m_txPower = data[1];
        m_needsDecrypting = false;
    }
    else {
        qDebug() << "Decrypted aem failed to validate";
    }

    EVP_CIPHER_CTX_free(ctx);

    return result;
}
