#ifndef ZIPISTREAMBUFFER_H
#define ZIPISTREAMBUFFER_H

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

#include <iostream>

// See https://stackoverflow.com/a/14086442
#define BUFFER_SIZE (1024)
class ZipIStreamBuffer : public std::streambuf {
public:
    ZipIStreamBuffer(QuaZip &quazip, QString const &filename);
    ~ZipIStreamBuffer();
    int underflow();
    bool success() const;
public:
    QuaZip &m_quazip;
    QuaZipFile m_file;
    char m_buffer[BUFFER_SIZE];
    bool m_result;
};

#endif // ZIPISTREAMBUFFER_H
