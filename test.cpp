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
    
};


int main() {
    return 0;
}