#include <cstddef>
#include <functional>
#include <stddef.h>
#include <stdint.h>
#include <iostream>
#include "string.h"


class string_view { //basically const string
    const char* buffer;
};

class string : public string_view{
    protected:
        char* buffer;
        size_t capacity_; // to avoid naming conflict 
        size_t length;
        char* append_impl(const char* str, size_t str_len) {
            size_t available_space = capacity_ - length;
            size_t to_copy = (str_len < available_space) ? str_len : available_space;
            if (str_len > available_space) {
                std::cerr << "Not enough capacity to append string\n";
                return nullptr;
            }
            for (size_t i = 0; i < to_copy; ++i) {
                buffer[length + i] = str[i];
            }
            length += to_copy;
            buffer[length] = '\0';
            return buffer;
        }

        string(size_t cap, char* buf)
            : buffer(buf), capacity_(cap), length(0) {
            buffer[0] = '\0';
        }

    public:
#ifdef STL

#endif

        string(const char *cstr) : string(strlen(cstr), new char[strlen(cstr) + 1]) {
            string::operator=(cstr);
        }

        string(const char *cstr, int size) : string(size, new char[size + 1]) {
            string::operator=(cstr);
        }

        string(const string& other) 
            : buffer(other.buffer), capacity_(other.capacity_), length(other.length) {
            if (other.length > capacity_) {
                printf("WARNING: String truncation in constructor! Cap: %zu, Src: %zu\n", capacity_, other.length);
            }

            size_t to_copy = (other.length < capacity_) ? other.length : capacity_;

            if (to_copy > 0) {
                memcpy(buffer, other.buffer, to_copy);
            }

            length = to_copy;
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
        string& operator=(const char* str) {
            if (str == nullptr) {
                length = 0;
                buffer[0] = '\0';
                return *this;
            }
            size_t str_len = strlen(str);
            // Check if the incoming string is longer than what we can hold
            if (str_len > capacity_) {
                printf("WARNING: String truncation! Source length %zu exceeds capacity %zu\n", str_len, capacity_);
            }
            size_t to_copy = (str_len < capacity_) ? str_len : capacity_;

            if (to_copy > 0) {
                memcpy(buffer, str, to_copy);
            }

            length = to_copy;
            buffer[length] = '\0';

            return *this;
        }
        string& operator=(const string& other) {
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
        bool concat(float num) {
            char num_str[32];
            int len = snprintf(num_str, sizeof(num_str), "%f", num);
            if (len < 0) return false; // encoding error
            return append_impl(num_str, static_cast<size_t>(len)) != nullptr;
        }
        bool concat(const string& other) {
            return append_impl(other.buffer, other.length) != nullptr;
        }

        bool operator+=(const string& other) {
            return concat(other);
        }
        bool operator==(const string& other) const {
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
        bool operator!=(const string& other) const {
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
        bool startsWith(const char* prefix) const {
            size_t i = 0;
            while (prefix[i] != '\0') {
                if (i >= length || buffer[i] != prefix[i]) return false;
                ++i;
            }
            return true;
        }
        int find(const char* substr) const {
            if (substr == nullptr || *substr == '\0') {
                return 0;
            }
            char* found = strstr(buffer, substr);
            if (found != nullptr) {
                return static_cast<int>(found - buffer);
            }
            return -1;
        }
        
        
        //find_first_of
        //replace
        virtual bool replace(const char* old_str, const char* new_str) {
            int index = find(old_str);
            if (index == -1) return false;

            size_t old_len = strlen(old_str);
            size_t new_len = strlen(new_str);
            size_t projected_len = length - old_len + new_len;
            if (projected_len > capacity_) {
                return false; 
            }   
            return string::replace(old_str, new_str);
        }
        void trim() {
            size_t start = 0;
            while (start < length && (buffer[start] == ' ' || buffer[start] == '\t' || buffer[start] == '\n' || buffer[start] == '\r')) {
                ++start;
            }
            size_t end = length;
            while (end > start && (buffer[end - 1] == ' ' || buffer[end - 1] == '\t' || buffer[end - 1] == '\n' || buffer[end - 1] == '\r')) {
                --end;
            }
            size_t new_length = end - start;
            for (size_t i = 0; i < new_length; ++i) {
                buffer[i] = buffer[start + i];
            }
            length = new_length;
            buffer[length] = '\0';
        }
        virtual ~string() {};

};


namespace str_utils {
    string to_string(int num) {
        char num_str[12];
        int len = snprintf(num_str, sizeof(num_str), "%d", num);
        if (len < 0) return string("");  // encoding error
        return string(num_str);
    }

    string to_string(float num) {
        char num_str[32];
        int len = snprintf(num_str, sizeof(num_str), "%.2f", num); 
        if (len < 0) return string(""); // encoding error
        return string(num_str); 
    }

    string to_string(const char* str) {
        return string(str);
    }

    string to_string(const char c) {
        char str[2] = {c, '\0'};
        return string(str);
    }
}

//std::print std::format

void print(const string_view &str) {

}

//string s;
//print(s);
//print("hello");
//print(F_("hello"));

//std::string ss;
//print(ss);


template <size_t N>
class FixedString : public string {
    public:
        FixedString() : string(storage, N) {}
        FixedString(const FixedString &other) : string(storage, N) {
            *this = other;
        }
    private:
        char storage[N+1];
};


class DynamicString : public string {
    public:
        DynamicString(size_t initial_capacity) : string(initial_capacity, new char[initial_capacity + 1]) {
            buffer[0] = '\0';
        }
        DynamicString(const string& other) 
            : string(new char[other.size() + 1], other.size()) {
            string::operator=(other);
        }
        // to override the copy constructor of string, otherwise it will just copy the pointer and cause double free issue
        DynamicString(const DynamicString& other) 
            : string(new char[other.capacity() + 1], other.capacity()) {
            string::operator=(other);
        }
        DynamicString(const char* cstr) : string(strlen(cstr), new char[strlen(cstr) + 1]) {
            string::operator=(cstr);
        }
        ~DynamicString() {
            delete[] buffer;
        }
        void resize(size_t min_capacity) {
            if (min_capacity <= capacity_) return;
            // used a hybrid approach for resizing that I think will be best for microcontrollers.
            size_t new_cap;
            if (capacity_ == 0) {
                new_cap = min_capacity; // Initial allocation
            } else if (capacity_ < 64) {
                new_cap = capacity_ * 2;
            } else {
                new_cap = capacity_ + 16; 
            }
            if (new_cap <= capacity_) return; // Never shrink
            if (new_cap < min_capacity) {
                new_cap = min_capacity + 16; // Fallback
            }
            char* new_buf = new char[new_cap + 1];            
            strcpy(new_buf, buffer);
            delete[] buffer;
            if (new_buf == nullptr) {
                std::cerr << "Failed to allocate memory for resizing string\n";
                return;
            }
            buffer = new_buf;
            capacity_ = new_cap;
        }
        bool concat(const char* str) {
            size_t str_len = strlen(str);
            if (length + str_len > capacity_) {
                resize(length + str_len);
            }
            return string::concat(str);
        }
        bool concat(char c) {
            if (length + 1 > capacity_) {
                resize(length + 1);
            }
            return string::concat(c);
        }

};

int main() {
    //FixedString<8> command = "move 100; delay(1000); stop";
    //command += "world";

    FixedString<16> str1;
    str1.concat("Hello world!!!!");
    std::cout << "str1: " << str1.c_str() << " (size: " << str1.size() << ", capacity: " << str1.capacity() << ")\n";
    return 0;
}