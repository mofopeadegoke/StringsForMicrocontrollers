#include <cstddef>
#include <cstdio>
#include <functional>
#include <cstring> // Required for memcpy, strstr, memcmp
#include <iostream>

class string_view { 
protected:
    const char* m_data; 
    size_t m_len;

public:
    string_view() : m_data(nullptr), m_len(0) {}
    string_view(const char* data, size_t len) : m_data(data), m_len(len) {}

    size_t size() const { return m_len; }
    const char* data() const { return m_data; }

    // Fast Access (Unchecked)
    char operator[](size_t index) const {
        return m_data[index];
    }
    
    // Safe Access (Checked)
    char at(size_t index) const {
        if (index >= m_len) {
            printf("ERROR: Index out of bounds!\n");
            return '\0'; 
        }
        return m_data[index];
    }

    bool operator==(const string_view& other) const {
        if (m_len != other.m_len) return false;
        return memcmp(m_data, other.m_data, m_len) == 0;
    }
    bool operator==(const char* str) const {
        if (!m_data || !str) return false;
        size_t str_len = strlen(str);
        if (m_len != str_len) return false;
        return memcmp(m_data, str, m_len) == 0;
    }
    bool operator!=(const string_view& other) const { return !(*this == other); }
    bool operator!=(const char* str) const { return !(*this == str); }

    int indexOf(char c) const {
        for (size_t i = 0; i < m_len; ++i) {
            if (m_data[i] == c) return static_cast<int>(i);
        }
        return -1;
    }

    bool startsWith(const char* prefix) const {
        size_t i = 0;
        while (prefix[i] != '\0') {
            if (i >= m_len || m_data[i] != prefix[i]) return false;
            ++i;
        }
        return true;
    }

    int find(const char* substr) const {
        if (substr == nullptr || *substr == '\0') return 0;
        const char* found = strstr(m_data, substr);
        if (found != nullptr) {
            return static_cast<int>(found - m_data);
        }
        return -1;
    }

    void print() const {
        if (m_data) std::cout << m_data;
    }
};

class string : public string_view {
    protected:
        char* buffer;
        size_t capacity_; 
        size_t length;
        bool m_owns_memory;

        void sync_view() {
            m_data = buffer;
            m_len = length;
        }

        char* append_impl(const char* str, size_t str_len) {
            size_t available_space = capacity_ - length;
            size_t to_copy = (str_len < available_space) ? str_len : available_space;
            
            if (str_len > available_space) {
                printf("WARNING: Not enough space to concatenate entire string. Truncating.\n");
            }
            
            memcpy(buffer + length, str, to_copy);
            length += to_copy;
            buffer[length] = '\0';
            
            sync_view();
            return buffer;
        }

        string(size_t cap, char* buf)
            : string_view(buf, 0), buffer(buf), capacity_(cap), length(0), m_owns_memory(false) {
            buffer[0] = '\0';
        }

    public:
        string(const char *cstr) 
            : string_view(nullptr, 0), capacity_(strlen(cstr)), length(0), m_owns_memory(true) 
        {
            buffer = new char[capacity_ + 1];
            string::operator=(cstr);
        }

        string(const char *cstr, int size) 
            : string_view(nullptr, 0), capacity_(size), length(0), m_owns_memory(true) 
        {
            buffer = new char[capacity_ + 1];
            if (size > 0) memcpy(buffer, cstr, size);
            length = size;
            buffer[length] = '\0';
            sync_view();
        }

        string(const string& other) 
            : string_view(nullptr, 0), 
            buffer(nullptr),         
            capacity_(other.capacity_),
            m_owns_memory(true)
        {
            buffer = new char[capacity_ + 1];    
            size_t to_copy = (other.length < capacity_) ? other.length : capacity_;
            
            if (to_copy > 0) {
                memcpy(buffer, other.buffer, to_copy);
            }

            length = to_copy;
            buffer[length] = '\0';
            sync_view();
        }

        virtual ~string() {
            if (m_owns_memory && buffer != nullptr) {
                delete[] buffer;
            }
        }

        size_t capacity() const { return capacity_; };
        char *c_str() const { return buffer; };     

        string& operator=(const char* str) {
            if (str == nullptr) {
                length = 0; buffer[0] = '\0'; sync_view(); return *this;
            }
            size_t str_len = strlen(str);
            size_t to_copy = (str_len < capacity_) ? str_len : capacity_;
            
            memcpy(buffer, str, to_copy);
            length = to_copy;
            buffer[length] = '\0';
            sync_view();
            return *this;
        }

        string& operator=(const string& other) {
            if (this != &other) {
                size_t to_copy = (other.length < capacity_) ? other.length : capacity_;
                memcpy(buffer, other.buffer, to_copy);
                length = to_copy;
                buffer[length] = '\0';
                sync_view();
            }
            return *this;
        }

        virtual bool concat(const char c) { return append_impl(&c, 1) != nullptr; }
        virtual bool concat(const char* str) { return append_impl(str, strlen(str)) != nullptr; }
        virtual bool concat(const string& other) { return append_impl(other.buffer, other.length) != nullptr; }

        virtual bool concat(int num) {
            char num_str[12];
            int len = snprintf(num_str, sizeof(num_str), "%d", num);
            if (len < 0) return false;
            return append_impl(num_str, static_cast<size_t>(len)) != nullptr;
        }

        bool operator+=(const string& other) { return concat(other); }
        
        virtual bool replace(const char* old_str, const char* new_str) {
            if (!old_str || !new_str) return false;
            int index = find(old_str);
            if (index == -1) return false;

            size_t old_len = strlen(old_str);
            size_t new_len = strlen(new_str);
            size_t new_total_len = length - old_len + new_len;
            
            if (new_total_len > capacity_) {
                printf("ERROR: Not enough capacity to replace string. Operation aborted.\n");
                return false; 
            }

            char* match_start = buffer + index;
            char* tail_start = match_start + old_len;
            size_t tail_len = length - (index + old_len);
            memmove(match_start + new_len, tail_start, tail_len);
            memcpy(match_start, new_str, new_len);
            length = new_total_len;
            buffer[length] = '\0';
            sync_view();      

            return true;
        }
};

template <size_t N>
class FixedString : public string {
    public:
        // Base constructor sets m_owns_memory = false
        FixedString() : string(N, storage) {}
        
        // Copy constructor manually copies data using operator=
        FixedString(const FixedString &other) : string(N, storage) {
            *this = other; 
        }

        // Allow constructing from C-string
        FixedString(const char* str) : string(N, storage) {
            *this = str;
        }

    private:
        char storage[N+1];
};

class DynamicString : public string {
    public:
        DynamicString(size_t initial_capacity) 
            : string(initial_capacity, new char[initial_capacity + 1]) {
            m_owns_memory = true;
            buffer[0] = '\0';
            sync_view();
        }

        DynamicString(const char* cstr) 
            : string(strlen(cstr), new char[strlen(cstr) + 1]) {
            m_owns_memory = true;
            string::operator=(cstr);
        }

        DynamicString(const string& other) 
            : string(new char[other.size() + 1], other.size()) { 
             m_owns_memory = true;
             string::operator=(other); 
        }

        ~DynamicString() {
            if (buffer) {
                delete[] buffer;
                buffer = nullptr;
            }
        }

        void resize(size_t min_capacity) {
            if (min_capacity <= capacity_) return;
            size_t new_cap;
            if (capacity_ == 0) {
                new_cap = min_capacity;
            } else if (capacity_ < 64) {
                new_cap = capacity_ * 2;
            } else {
                new_cap = capacity_ + 16; 
            }
            
            if (new_cap < min_capacity) {
                new_cap = min_capacity + 16; 
            }

            char* new_buf = new char[new_cap + 1];
            if (!new_buf) return;

            if (length > 0) {
                memcpy(new_buf, buffer, length);
            }
            new_buf[length] = '\0';

            delete[] buffer;

            buffer = new_buf;
            capacity_ = new_cap;
            
            sync_view();
        }

        
        bool concat(const char* str) {
            if (str == nullptr) return false;
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
        
        // Note: For int/float, we can rely on the base class because 
        // string::concat(int) eventually calls string::concat(char*), 
        // which might resolve back to here if we make it virtual, 
        // OR we should override them here too to be safe.
        // For now, these two cover the raw string building blocks.
};

// Global Print using View
void print(const string_view &str) {
    str.print();
    std::cout << "\n";
}

int main() {
    FixedString<50> fstr = "Hello";
    fstr.concat(" World");
    
    // Test View Functionality
    if (fstr.startsWith("Hello")) {
        std::cout << "Starts with Hello!\n";
    }
    
    print(fstr); // Works!
    return 0;
}