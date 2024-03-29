#pragma once

#include "from_chars.h"
#include <istream>
#include <sstream>
#include <tuple>
#include <array>

namespace injector::detail {

struct const_membuf : public std::stringbuf {
    const_membuf(char const *base, size_t size)
        : std::stringbuf(std::ios::in | std::ios::binary) {
        char *ptr(const_cast<char *>(base));
        this->setg(ptr, ptr, ptr + size);
    }
};

struct resource_stream : virtual const_membuf, virtual std::istream {
    resource_stream(char const *base, size_t size)
        : const_membuf(base, size), std::istream(static_cast<std::streambuf *>(this)) {
    }

    [[nodiscard]]
    void const * data() const {
        return this->eback();
    }

    [[nodiscard]]
    int size() const {
        return static_cast<int>(this->egptr() - this->eback());
    }
};

class compile_time_stream {
 public:
    constexpr compile_time_stream(const char *data, size_t data_size)
        : last_error_(parse_error_code::NO_ERROR), begin_(data), curr_(data), end_(data + data_size) {}

    constexpr compile_time_stream &operator>>(int &c) {
        if (prepare()) {
            std::tie(curr_, last_error_) = from_chars(curr_, end_, c);
        }
        return *this;
    }

    constexpr compile_time_stream &operator>>(char &c) {
        if (prepare()) {
            std::tie(curr_, last_error_) = from_chars(curr_, end_, c);
        }
        return *this;
    }

    constexpr compile_time_stream &operator>>(double &c) {
        if (prepare()) {
            std::tie(curr_, last_error_) = from_chars(curr_, end_, c);
        }
        return *this;
    }

    constexpr compile_time_stream &operator>>(std::string_view &c) {
        if (prepare()) {
            char const *begin = curr_;
            while (curr_ < end_ && !is_blank(*curr_)) {
                ++curr_;
            }
            c = std::string_view(begin, curr_);
        }
        return *this;
    }

    template <size_t N>
    constexpr compile_time_stream &operator>>(char (&array)[N]) {
        read_array<N>(array);
        return *this;
    }

    template <size_t N>
    constexpr compile_time_stream &operator>>(std::array<char, N> &array) {
        read_array<N>(array.data());
        return *this;
    }

    constexpr void skip(size_t n) {
        curr_ = std::min(curr_ + n, end_);
    }

    constexpr void set_pos(size_t pos) {
        curr_ = begin_;
        skip(pos);
    }

    constexpr size_t current_pos() {
        return static_cast<size_t>(curr_ - begin_);
    }

    constexpr void set_error(parse_error_code error) {
        last_error_ = error;
    }

    [[nodiscard]] constexpr std::size_t rest_size() const {
        return end_ - curr_;
    }

    [[nodiscard]] constexpr std::size_t size() const {
        return end_ - begin_;
    }

    [[nodiscard]] constexpr char const *data() const {
        return begin_;
    }

    [[nodiscard]] constexpr std::string_view to_string_view() const {
        return {begin_, end_};
    }

    [[nodiscard]] constexpr std::string_view to_string_view_rest() const {
        return {curr_, end_};
    }

    [[nodiscard]] constexpr parse_error_code last_error() const {
        return last_error_;
    }

    constexpr bool eof() {
        if (!prepare()) {
            return false;
        }
        return curr_ == end_;
    }

 private:
    template <size_t N>
    constexpr void read_array(char * array) {
        if (rest_size() < N) {
            last_error_ = parse_error_code::NO_CHARS;
            return;
        }
        for (size_t i = 0; i < N; ++i) {
            array[i] = curr_[i];
        }
        curr_ += N;
    }

    constexpr bool prepare() {
        if (last_error_ != parse_error_code::NO_ERROR) {
            return false;
        }
        while (curr_ < end_ && is_blank(*curr_)) {
            ++curr_;
        }
        return true;
    }

    constexpr static bool is_blank(char c) {
        return c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\0';
    }

    parse_error_code last_error_;
    char const *const begin_;
    char const *curr_;
    char const *const end_;
};

}// namespace injector::detail
