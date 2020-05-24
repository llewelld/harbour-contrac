#include "zipistreambuffer.h"

using namespace std;

ZipIStreamBuffer::ZipIStreamBuffer(QuaZip &quazip, QString const &filename)
    : m_quazip(quazip)
    , m_file(&quazip)
{
    m_result = m_quazip.setCurrentFile(filename);
    if (m_result) {
        m_result = m_file.open(QIODevice::ReadOnly);
    }
}

ZipIStreamBuffer::~ZipIStreamBuffer()
{
    if (m_result) {
        m_file.close();
    }
}

bool ZipIStreamBuffer::success() const
{
    return m_result;
}

int ZipIStreamBuffer::underflow() {
    int result;
    if (gptr() == egptr()) {
        qint64 readCount;

        readCount = m_file.read(m_buffer, BUFFER_SIZE);
        if (readCount >= 0) {
            setg(m_buffer, m_buffer, m_buffer + readCount);
        }
    }
    result = (gptr() == egptr()) ? char_traits<char>::eof() : char_traits<char>::to_int_type(*gptr());
    return result;
}

