#pragma once

#include <set>
#include <cassert>
#include <string>

struct COsiHashedString {
    uint32_t hash;
    std::string str;

    static uint32_t Hash(const unsigned char *str) {
        uint32_t hash = 0;
        unsigned char c;
        while ((c = *str++)) {
            hash = (c | 0x20) + 129 * (hash % 4294967);
        }
        return hash;
    }

    static std::string FormatFunctionSignature(const unsigned char *name, uint32_t numArgs) {
        char buf[16];
        sprintf(buf, "/%-d", numArgs);

        std::string str = (const char *)name;
        str += buf;
        return str;
    }

    static uint32_t HashFunctionSignature(const unsigned char *name, uint32_t numArgs) {
        return Hash(name) + numArgs;
    }

    COsiHashedString(uint32_t hash, std::string str)
        : hash(hash),
          str(str) {}

    // for function signatures
    COsiHashedString(const unsigned char *name, uint32_t numArgs)
        : COsiHashedString(HashFunctionSignature(name, numArgs), FormatFunctionSignature(name, numArgs)) {
    }

    COsiHashedString(const unsigned char *chars)
        : COsiHashedString(Hash(chars), std::string((const char *)chars)) {}
};

template <typename T>
class COsiSearchIndex {
    struct Entry {
        std::string key;
        T value;

        bool operator<(const Entry &other) const {
            return stricmp(key.c_str(), other.key.c_str()) < 0;
        }
    };

    constexpr static size_t NUM_BUCKETS = 0x3FF;

    std::set<Entry> m_buckets[NUM_BUCKETS];
    size_t m_size;

public:
    class iterator : public std::forward_iterator_tag {
        friend class COsiSearchIndex<T>;

        uint32_t m_bucketIndex;
        typename std::set<Entry>::iterator m_setIt;
        const COsiSearchIndex<T> *m_pMyIndex;

    public:
        bool operator!=(const iterator &other) const {
            return other.m_bucketIndex != m_bucketIndex ||
                   other.m_setIt != m_setIt ||
                   other.m_pMyIndex != m_pMyIndex;
        }

        bool operator==(const iterator &other) const {
            return other.m_bucketIndex == m_bucketIndex &&
                   other.m_setIt == m_setIt &&
                   other.m_pMyIndex == m_pMyIndex;
        }

        const T &operator*() const {
            assert(*this != m_pMyIndex->end());
            return m_setIt->value;
        }
    };

    COsiSearchIndex() : m_size(0) {}

    bool set(const COsiHashedString &key, T value) {
        iterator pElem = lookup(key.hash, key.str);
        if (pElem != end()) {
            delete *pElem;
            erase(pElem);
        }
        return insert(key.hash, key.str, value);
    }

    void erase(iterator pElem) {
        assert(pElem.m_pMyIndex == this);
        if (pElem != end()) {
            m_buckets[pElem.m_bucketIndex].erase(pElem.m_setIt);
            --m_size;
        }
    }

    iterator begin() const {
        iterator it;
        it.m_bucketIndex = 0;
        it.m_setIt       = m_buckets[0].begin();
        it.m_pMyIndex    = this;

        return it;
    }

    iterator end() const {
        iterator it;
        it.m_bucketIndex = NUM_BUCKETS - 1;
        it.m_setIt       = m_buckets[NUM_BUCKETS - 1].end();
        it.m_pMyIndex    = this;

        return it;
    }

    T lookupPtr(const COsiHashedString &key) const {
        iterator it = lookup(key.hash, key.str);
        if (it == end()) {
            return nullptr;
        } else {
            return *it;
        }
    }

private:
    iterator lookup(uint32_t hash, const std::string &str) const {
        iterator itOut;
        itOut.m_bucketIndex = hash % NUM_BUCKETS; // ORIGINAL BUG: actually it should be [hash % (NUM_BUCKETS - 1)]

        auto &bucket  = m_buckets[itOut.m_bucketIndex];
        itOut.m_setIt = bucket.find({ str, nullptr });
        if (itOut.m_setIt == bucket.end()) {
            itOut.m_bucketIndex = NUM_BUCKETS - 1;
        }

        itOut.m_pMyIndex = this;

        return itOut;
    }

    bool insert(uint32_t keyHash, const std::string &str, const T &value) {
        auto &bucket = m_buckets[keyHash % NUM_BUCKETS];
        Entry entry{ str, value };

        if (!bucket.insert(entry).second) return false;
        m_size++;
        return true;
    }
};
