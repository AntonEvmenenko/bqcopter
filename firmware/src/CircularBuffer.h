#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <inttypes.h>

template <typename T, uint16_t Size>
class CircularBuffer {
public:
    CircularBuffer() :
        realSize(0), startIndex(0), endIndex(0) {}
    ~CircularBuffer() {}

    uint16_t get_size() {
        return realSize;
    }

    void push_back(T newElement) {
        data[endIndex] = newElement;
        endIndex = (endIndex + 1) % Size;
        if (realSize == Size) {
            startIndex = (startIndex + 1) % Size;
        } else {
            realSize++;
        }
    }

    T get(uint16_t index) {
        return data[(startIndex + index) % Size];
    }

    T get_last() {
        return get(get_size() - 1);
    }

    T get_first() {
        return get(0);
    }

    void remove_first() {
        if(realSize > 0) {
            --realSize;
            startIndex = (startIndex + 1) % Size;
        }
    }

    void remove_last() {
        if(realSize > 0) {
            --realSize;
        }
    }

    T pop_front() {
        T temp = get_first();
        remove_first();
        return temp;
    }

    T pop_back() {
        T temp = get_last();
        remove_last();
        return temp;
    }

    void clear() {
        realSize = 0;
        endIndex = startIndex;
    }

private:
    T data[Size];
    uint16_t realSize;
    uint16_t startIndex;
    uint16_t endIndex;
};

#endif // CIRCULARBUFFER_H
