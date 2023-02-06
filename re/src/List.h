#pragma once

#include <list>

class CSelfPointer {

};

template<typename T>
class CList : public std::list<T> {};

template<typename T>
class CPointerList : public CList<T> {
public:
    virtual ~CPointerList() = default;

    void ClearPointerList() {
        for (auto it = this->begin(); it != this->end(); ++it) {
            delete *it;
        }
        this->clear();
    }
};
