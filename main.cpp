#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <algorithm>
#include <string_view>

class StaticStringBase {
protected:
    char* buffer_;
    size_t capacity_;
    size_t size_;

public:
    StaticStringBase(char* buf, size_t cap) 
        : buffer_(buf), capacity_(cap), size_(0) {
        buffer_[0] = '\0';
    }

    StaticStringBase& operator=(const StaticStringBase& other) {
        if (this != &other) {
            size_ = 0;
            buffer_[0] = '\0';
            append(other.c_str());
        }
        return *this;
    }

    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }
    bool empty() const { return size_ == 0; }
    void append(std::string_view sv) {
        size_t len = sv.length();
        size_t available = capacity_ - size_;
        size_t to_copy = (len < available) ? len : available;
        memmove(buffer_ + size_, sv.data(), to_copy);
        size_ += to_copy;
        buffer_[size_] = '\0';
    }

    void push_back(char c) {
        if (size_ < capacity_) {
            buffer_[size_] = c;
            size_++;
            buffer_[size_] = '\0';
        }
    }
    operator std::string_view() const { return std::string_view(buffer_, size_); }
    
    const char* c_str() const { return buffer_; }

    StaticStringBase& operator+=(char c) { push_back(c); return *this; }
    StaticStringBase& operator+=(const char* str) { append(str); return *this; }
    StaticStringBase& operator+=(std::string_view sv) { append(sv); return *this; }

    char& operator[](size_t index) { return buffer_[index]; }
    char at(size_t index) const { return (index < size_) ? buffer_[index] : '\0'; }
    bool operator==(const StaticStringBase& other) const {
        return std::string_view(*this) == std::string_view(other);
    }
    bool operator==(const char* other) const {
        return std::string_view(*this) == other;
    }
};

template <size_t N>
class FixedString : public StaticStringBase {
private:
    char storage_[N + 1];

public:
    FixedString() : StaticStringBase(storage_, N) {
        storage_[0] = '\0';
    }
    FixedString(const char* str) : StaticStringBase(storage_, N) {
        append(str);
    }

    FixedString(const StaticStringBase& other) : StaticStringBase(storage_, N) {
        append(other); 
    }
    FixedString& operator=(const StaticStringBase& other) {
        StaticStringBase::operator=(other);
        return *this;
    }

    FixedString& operator=(const char* str) {
        size_ = 0;
        buffer_[0] = '\0';
        append(str);
        return *this;
    }
};