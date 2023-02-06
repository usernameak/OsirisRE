#pragma once

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "OsiCommon.h"

class COsiSmartBuf {
public:
    uint8_t m_xorValue;

private:
    FILE *m_file;
    uint8_t *m_buffer;
    uint8_t *m_curPosition;
    size_t m_bufferCapacity;
    size_t m_blockSize;
    size_t m_bufferSize;
    uint8_t m_osiFileVersion;

public:
    COsiSmartBuf(const char *filename, size_t blockSize, uint8_t osiFileVersion)
        : m_blockSize(blockSize),
          m_osiFileVersion(osiFileVersion) {
        m_file = fopen(filename, "wb");
        Init();

        WriteUint8(m_osiFileVersion);
    }

    COsiSmartBuf(const char *filename, size_t blockSize)
        : m_blockSize(blockSize) {
        m_file = fopen(filename, "rb");
        Init();

        ReadUint8(&m_osiFileVersion);
    }

    ~COsiSmartBuf() {
        Flush();
        fclose(m_file);

        free(m_buffer);
    }

    bool Flush() {
        size_t bytesToWrite = m_curPosition - m_buffer;
        size_t bytesWritten = fwrite(m_buffer, bytesToWrite, 1u, m_file);

        m_curPosition = m_buffer;
        m_bufferSize  = 0;

        return bytesWritten != bytesToWrite; // FIXME: seems like a bug? double-check it later
    }

    bool WriteUint8(uint8_t value) {
        *this->m_curPosition++ = value;
        return FlushNextBlock();
    }

    bool ReadUint8(uint8_t *value) {
        if (!FetchByte()) return false;

        *value = *this->m_curPosition++;
        return true;
    }

    bool ReadBytes(void *buf, size_t size) {
        if (!FetchBytes(size)) return false;

        memcpy(buf, this->m_curPosition, size);
        this->m_curPosition += size;
        return true;
    }

    bool WriteBytes(const void *buf, size_t size) {
        memcpy(this->m_curPosition, buf, size);
        this->m_curPosition += size;

        return FlushNextBlock();
    }

    bool WriteUint32(uint32_t value) {
        WriteBytes(&value, 4);

        return FlushNextBlock(); // ORIGINAL BUG: FlushNextBlock is called twice, error handling issues
    }

    bool ReadUint8_2(uint8_t *value) {
        if (!FetchBytes(1)) return false; // ORIGINAL BUG: FetchBytes is called twice

        ReadBytes(value, 1);
        return true;
    }

    bool WriteUint8_2(uint8_t value) {
        WriteBytes(&value, 1);

        return FlushNextBlock(); // ORIGINAL BUG: FlushNextBlock is called twice, error handling issues
    }

    bool ReadInt(int *value) {
        if (!FetchBytes(4)) return false; // ORIGINAL BUG: FetchBytes is called twice

        ReadBytes(value, 4);
        return true;
    }

    bool ReadUint32(uint32_t *value) {
        if (!FetchBytes(4)) return false; // ORIGINAL BUG: FetchBytes is called twice

        ReadBytes(value, 4);
        return true;
    }

    bool ReadEncString(unsigned char *buf) {
        while (COsiSmartBuf::ReadUint8(buf)) {
            *buf ^= m_xorValue;
            if (!*buf)
                return true;

            buf++;
        }
        return false;
    }

    bool ReadEncString(unsigned char **ppStr) {
        unsigned char temp[1024];
        unsigned char *pCurChar = temp;
        while (true) {
            if (!ReadUint8(pCurChar)) return false;

            *pCurChar ^= m_xorValue;
            if (!*pCurChar++) break;
        }

        if (*ppStr) {
            free(*ppStr);
            *ppStr = nullptr;
        }

        size_t strLen = pCurChar - temp + 1;

        auto *outString = (unsigned char *)malloc(strLen);
        memset(outString, 0, strLen);
        *ppStr = outString;
        memcpy(outString, temp, strLen);
        return true;
    }

    bool WriteEncString(const unsigned char *str) {
        const unsigned char *curChar = str;
        do {
            unsigned char c = m_xorValue ^ *++curChar;
            if (!WriteUint8(c)) return false;
        } while (*curChar);
        return true;
    }

private:
    void Init() {
        if (m_file) {
            m_bufferCapacity = 2 * m_blockSize;
            m_buffer         = (uint8_t *)malloc(m_bufferCapacity);
            m_curPosition    = m_buffer;
            m_bufferSize     = 0;

            g_OsiHadError = false;
            m_xorValue = 0;

            memset(m_buffer, 0, m_bufferCapacity); // why do they memset it? it's not read until populated anyway...
        } else {
            m_bufferCapacity = 0;
            m_buffer         = nullptr;
            m_curPosition    = nullptr;
            m_bufferSize     = 0;

            g_OsiHadError = true;
            m_xorValue = 0;
        }
    }

    bool FlushNextBlock() {
        if (m_curPosition - m_buffer < m_blockSize) return true;

        if (fwrite(m_buffer, 1u, m_blockSize, m_file) != m_blockSize)
            return false;

        size_t remainingSize = m_curPosition - m_blockSize - m_buffer;

        memcpy(m_buffer, m_buffer + m_blockSize, remainingSize);
        m_bufferSize = remainingSize;

        m_curPosition = m_buffer + remainingSize;

        return true;
    }

    void FetchNextBlock() {
        size_t remainingSize = m_bufferSize + m_buffer - m_curPosition;

        memcpy(m_buffer, m_curPosition, remainingSize);

        uint32_t bytesRead = fread(&m_buffer[remainingSize],
            1u, m_blockSize,
            m_file);
        m_bufferSize       = remainingSize + bytesRead;
        m_curPosition      = m_buffer;
    }

    bool FetchByte() {
        if (m_curPosition - m_buffer < m_bufferSize)
            return true;

        FetchNextBlock();

        return m_buffer + m_bufferSize - m_curPosition >= 1;
    }

    bool FetchBytes(size_t size) {
        if (size <= this->m_bufferSize + this->m_buffer - this->m_curPosition)
            return true;

        FetchNextBlock();

        return m_buffer + m_bufferSize - m_curPosition >= size;
    }
};
