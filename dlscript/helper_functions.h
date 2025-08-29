#pragma once
#include <cctype>
#include <string>
#include <vector>

static inline bool is_digit(char c) { return std::isdigit(static_cast<unsigned char>(c)) != 0; }
static inline bool is_hex_digit(char c) { return std::isxdigit(static_cast<unsigned char>(c)) != 0; }
static inline bool is_bin_digit(char c) { return c == '0' || c == '1'; }
static inline bool is_oct_digit(char c) { return c >= '0' && c <= '7'; }
static inline bool is_space(char c) { return std::isspace(static_cast<unsigned char>(c)) != 0; }

static inline bool is_ident_start(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}
static inline bool is_ident_continue(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

static inline bool is_identifier(const std::string& s) {
    if (s.empty()) return false;
    if (!is_ident_start(s[0])) return false;
    for (size_t i = 1; i < s.size(); ++i) {
        if (!is_ident_continue(s[i])) return false;
    }
    return true;
}

static inline bool unescape_string_body(const std::string& body, char quote, std::string& out)
{
    out.clear();
    out.reserve(body.size());
    for (size_t i = 0; i < body.size(); ++i) {
        char c = body[i];
        if (c == '\\') {
            if (i + 1 >= body.size()) return false;
            char e = body[++i];
            switch (e) {
            case 'n': out.push_back('\n'); break;
            case 'r': out.push_back('\r'); break;
            case 't': out.push_back('\t'); break;
            case 'b': out.push_back('\b'); break;
            case 'f': out.push_back('\f'); break;
            case 'v': out.push_back('\v'); break;
            case '0': out.push_back('\0'); break;
            case '\\': out.push_back('\\'); break;
            case '\'': out.push_back('\''); break;
            case '\"': out.push_back('\"'); break;
            case 'x': {
                unsigned v = 0, cnt = 0;
                while (cnt < 2 && i + 1 < body.size() && is_hex_digit(body[i + 1])) {
                    char h = body[++i];
                    v = (v << 4) |
                        (h <= '9' ? (h - '0') :
                            h <= 'F' ? (h - 'A' + 10) :
                            (h - 'a' + 10));
                    ++cnt;
                }
                if (cnt == 0) return false;
                out.push_back(static_cast<char>(v));
            } break;
            default: return false;
            }
        }
        else {
            if (c == quote) return false;
            out.push_back(c);
        }
    }
    return true;
}

struct word_span {
    std::string text;
    size_t start;
    size_t end;   // [start, end)
    std::string to_string() const { return text; }
};

static inline bool next_word_ws(std::string_view s, size_t& i, word_span& out) {
    const size_t n = s.size();

    while (i < n && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
    if (i >= n) return false;

    if (s[i] == ';') {
        const size_t start = i++;
        out = word_span{ std::string(s.substr(start, 1)), start, start + 1 };
        return true;
    }

    const size_t start = i;
    while (i < n && !std::isspace(static_cast<unsigned char>(s[i])) && s[i] != ';') ++i;

    out = word_span{ std::string(s.substr(start, i - start)), start, i };
    return true;
}

static inline std::vector<word_span> words_ws(std::string_view s) {
    std::vector<word_span> v;
    size_t i = 0;
    word_span w;
    while (next_word_ws(s, i, w)) v.push_back(w);
    return v;
}
