#include "data_types.h"
#include "helper_functions.h"

bool is_const_integer_value(const std::string& tok, int64_t& value)
{
    value = 0;
    if (tok.empty()) return false;

    size_t i = 0, n = tok.size();
    bool neg = false;

    if (tok[i] == '+' || tok[i] == '-') {
        neg = (tok[i] == '-');
        if (++i >= n) return false;
    }

    int base = 10;
    if (i + 1 < n && tok[i] == '0') {
        char p = tok[i + 1];
        if (p == 'x' || p == 'X') { base = 16; i += 2; }
        else if (p == 'b' || p == 'B') { base = 2; i += 2; }
        else if (p == 'o' || p == 'O') { base = 8; i += 2; }
    }
    if (i >= n) return false;

    const unsigned long long max_mag =
        neg ? (static_cast<unsigned long long>(LLONG_MAX) + 1ull)
        : static_cast<unsigned long long>(LLONG_MAX);

    unsigned long long mag = 0;
    bool any = false;

    for (; i < n; ++i) {
        char c = tok[i];
        int d = -1;

        if (base == 10) {
            if (is_digit(c)) d = c - '0';
        }
        else if (base == 2) {
            if (is_bin_digit(c)) d = c - '0';
        }
        else if (base == 8) {
            if (is_oct_digit(c)) d = c - '0';
        }
        else {
            if (is_digit(c)) d = c - '0';
            else if (c >= 'a' && c <= 'f') d = 10 + (c - 'a');
            else if (c >= 'A' && c <= 'F') d = 10 + (c - 'A');
        }

        if (d < 0) return false;
           any = true;

        if (mag > (max_mag - static_cast<unsigned long long>(d)) / static_cast<unsigned long long>(base))
            return false;

        mag = mag * static_cast<unsigned long long>(base) + static_cast<unsigned long long>(d);
    }

    if (!any) return false;

    if (neg) {
        if (mag == static_cast<unsigned long long>(LLONG_MAX) + 1ull)
            value = LLONG_MIN;
        else
            value = -static_cast<int64_t>(mag);
    }
    else {
        value = static_cast<int64_t>(mag);
    }
    return true;
}

bool is_const_floating_value(const std::string& tok, double& value)
{
    value = 0.0;
   
    if (tok.empty()) return false;

    size_t n = tok.size();
    bool has_suffix = (tok.back() == 'f' || tok.back() == 'F' ||
        tok.back() == 'l' || tok.back() == 'L');
   
    size_t end = has_suffix ? n - 1 : n;
   
    if (end == 0) 
        return false;

    size_t i = 0;
    if (tok[i] == '+' || tok[i] == '-') {
        if (++i >= end) return false;
    }

    bool any_digit = false;
    bool seen_dot = false;
    bool seen_exp = false;

    while (i < end) {
        char c = tok[i];

        if (is_digit(c)) { any_digit = true; ++i; continue; }

        if (c == '.' && !seen_dot && !seen_exp) {
            seen_dot = true;
            ++i;
            continue;
        }

        if ((c == 'e' || c == 'E') && !seen_exp && any_digit) {
            seen_exp = true;
            ++i; 

            if (i < end && (tok[i] == '+' || tok[i] == '-')) 
                ++i;

            if (i >= end || !is_digit(tok[i]))
                return false;

            while (i < end && is_digit(tok[i])) ++i;
            continue;
        }

        return false; 
    }

    if (!any_digit || (!seen_dot && !seen_exp))
        return false;

    std::string tmp(tok.begin(), tok.begin() + end);
    char* endp = nullptr;
    errno = 0;
    value = std::strtod(tmp.c_str(), &endp);
    if (endp == tmp.c_str() || *endp != '\0' || errno == ERANGE) return false;

    return true;
}



bool is_const_string_value(const std::string& tok, std::string& value)
{
    value.clear();
    if (tok.size() < 2) return false;
    char q = tok.front();
    if ((q != '"' && q != '\'') || tok.back() != q) return false;
    std::string body = tok.substr(1, tok.size() - 2);
    return unescape_string_body(body, q, value);
}