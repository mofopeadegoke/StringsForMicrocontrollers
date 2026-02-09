#include <cstddef>
#include <stddef.h>
#include <stdint.h>
#include <iostream>

class StaticStringBase {
    protected:
        char* buffer;
        size_t capacity_; // to avoid naming conflict 
        size_t length;
        // supposed to return a pointer or a boolean? what do you think sir? 
        char* append_impl(const char* str, size_t str_len) {
            size_t available_space = capacity_ - length;
            size_t to_copy = (str_len < available_space) ? str_len : available_space;
            for (size_t i = 0; i < to_copy; ++i) {
                buffer[length + i] = str[i];
            }
            length += to_copy;
            buffer[length] = '\0';
            return buffer;
        }

    public:
        StaticStringBase(char* buf, size_t cap)
            : buffer(buf), capacity_(cap), length(0) {
            buffer[0] = '\0';
        }

        StaticStringBase(const StaticStringBase& other)
            : buffer(other.buffer), capacity_(other.capacity_), length(other.length) {
            for (size_t i = 0; i < length; ++i) {
                buffer[i] = other.buffer[i];
            }
            buffer[length] = '\0';
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
        bool concat(const char c) {
            return append_impl(&c, 1) != nullptr;
        }
        bool concat(const char* str) {
            size_t str_len = 0;
            while (str[str_len] != '\0') {
                ++str_len;
            }
            return append_impl(str, str_len) != nullptr;
        }
        bool concat(int num) {
            char num_str[12];
            int len = snprintf(num_str, sizeof(num_str), "%d", num);
            if (len < 0) return false; // encoding error
            return append_impl(num_str, static_cast<size_t>(len)) != nullptr;
        }
        bool concat(const StaticStringBase& other) {
            return append_impl(other.buffer, other.length) != nullptr;
        }
        bool operator+=(const char c) {
            return concat(c);
        }
        bool operator+=(const char* str) {
            return concat(str);
        }
        bool operator+=(int num) {
            return concat(num);
        }
        bool operator+=(const StaticStringBase& other) {
            return concat(other);
        }
        bool operator==(const StaticStringBase& other) const {
            if (length != other.length) return false;
            for (size_t i = 0; i < length; ++i) {
                if (buffer[i] != other.buffer[i]) return false;
            }
            return true;
        }
        bool operator==(const char* str) const {
            size_t i = 0;
            while (str[i] != '\0' && i < length) {
                if (buffer[i] != str[i]) return false;
                ++i;
            }
            return str[i] == '\0' && i == length;
        }
        bool operator!=(const StaticStringBase& other) const {
            return !(*this == other);
        }
        bool operator!=(const char* str) const {
            return !(*this == str);
        }
        int indexOf(char c) const {
            for (size_t i = 0; i < length; ++i) {
                if (buffer[i] == c) return static_cast<int>(i);
            }
            return -1;
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