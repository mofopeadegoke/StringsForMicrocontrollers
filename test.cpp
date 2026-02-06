#include <cstddef>
#include <stddef.h>
#include <stdint.h>
#include <iostream>

class StaticStringBase {
    protected:
        char* buffer;
        size_t capacity_; // to avoid naming conflict 
        size_t length;

    public:
        StaticStringBase(char* buf, size_t cap)
            : buffer(buf), capacity_(cap), length(0) {
            buffer[0] = '\0';
        }
        size_t size() const { return length; };
        size_t capacity() const { return capacity_; };
        char *c_str() const { return buffer; };

        char operator[](size_t index) const {
            if (index < length) {
                return buffer[index];
            }
            return '\0';
        }
        StaticStringBase& operator=(const char* str) {
            size_t i = 0;
            while (str[i] != '\0' && i < capacity_) {
                buffer[i] = str[i];
                ++i;
            }
            length = i;
            buffer[length] = '\0';
            return *this;
        }
        StaticStringBase& operator=(const StaticStringBase& other) {
            if (this != &other) {
                size_t i = 0;
                while (i < other.length && i < capacity_) {
                    buffer[i] = other.buffer[i];
                    ++i;
                }
                length = i;
                buffer[length] = '\0';
            }
            return *this;
        }

};


template <size_t N>
class FixedString : public StaticStringBase {
    public:
        FixedString() : StaticStringBase(storage, N) {}
        FixedString(const FixedString &other) : StaticStringBase(storage, N) {
            *this = other;
        }
    private:
        char storage[N+1];
};


int main() {
    return 0;
}