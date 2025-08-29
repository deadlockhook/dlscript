#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <cctype>
#include "helper_functions.h"

struct word_span {
    std::string text;   
    size_t start;    
    size_t end;
    std::string to_string() const { return text; }
};


static inline bool match3(std::string_view s, size_t i, std::string& out) {
    if (i + 3 > s.size()) return false;
    auto a = s[i], b = s[i + 1], c = s[i + 2];
    if ((a == '<' && b == '<' && c == '=') || (a == '>' && b == '>' && c == '=')) {
        out.assign(s.substr(i, 3));
        return true;
    }
    return false;
}

static inline bool match2(std::string_view s, size_t i, std::string& out) {
    if (i + 2 > s.size()) return false;
    const char a = s[i], b = s[i + 1];
    switch (a) {
    case '=': if (b == '=') { out = "=="; return true; } break;
    case '!': if (b == '=') { out = "!="; return true; } break;
    case '<': if (b == '=') { out = "<="; return true; } if (b == '<') { out = "<<"; return true; } break;
    case '>': if (b == '=') { out = ">="; return true; } if (b == '>') { out = ">>"; return true; } break;
    case '&': if (b == '&') { out = "&&"; return true; } if (b == '=') { out = "&="; return true; } break;
    case '|': if (b == '|') { out = "||"; return true; } if (b == '=') { out = "|="; return true; } break;
    case '+': if (b == '+') { out = "++"; return true; } if (b == '=') { out = "+="; return true; } break;
    case '-': if (b == '-') { out = "--"; return true; } if (b == '=') { out = "-="; return true; } break;
    case '*': if (b == '=') { out = "*="; return true; } break;
    case '/': if (b == '=') { out = "/="; return true; } break;
    case '%': if (b == '=') { out = "%="; return true; } break;
    case '^': if (b == '=') { out = "^="; return true; } break;
    default: break;
    }
    return false;
}

static inline bool is_single_delim(char c) {
    switch (c) {
    case '+': case '-': case '*': case '/': case '%':
    case '=': case '!': case '<': case '>':
    case '&': case '|': case '^': case '~':
    case '(': case ')': case '{': case '}':
    case '[': case ']': case ';': case ',': case '?': case ':': case '.':
        return true;
    default: return false;
    }
}

// number scanner so 1.23/.5/1e-3/0.0f stay one token; returns end index (exclusive)
static size_t scan_number(std::string_view s, size_t i) {
    const size_t n = s.size();
    size_t j = i;
    bool seen_digit = false, seen_dot = false;

    auto isdig = [&](size_t k) { return k < n && std::isdigit((unsigned char)s[k]); };

    if (s[j] == '.') {
        if (!isdig(j + 1)) return i; 
        seen_dot = true; j++; while (isdig(j)) j++;
    }
    else {
        while (isdig(j)) { seen_digit = true; j++; }
        if (j < n && s[j] == '.') { seen_dot = true; j++; while (isdig(j)) j++; }
    }

    if (j < n && (s[j] == 'e' || s[j] == 'E')) {
        size_t k = j + 1;
        if (k < n && (s[k] == '+' || s[k] == '-')) ++k;
        if (k < n && std::isdigit((unsigned char)s[k])) {
            j = k + 1;
            while (j < n && std::isdigit((unsigned char)s[j])) ++j;
        }
    }

    if (j < n && (s[j] == 'f' || s[j] == 'F' || s[j] == 'l' || s[j] == 'L')) ++j;

    return (j > i) ? j : i;
}

static inline std::vector<word_span> tokenize_line_ops(std::string_view s, bool& in_block_comment)
{
    std::vector<word_span> out;
    const size_t n = s.size();
    size_t i = 0;

    auto push = [&](size_t a, size_t b) {
        out.push_back(word_span{ std::string(s.substr(a, b - a)), a, b });
        };
    auto skip_ws = [&] { while (i < n && std::isspace((unsigned char)s[i])) ++i; };

    while (i < n) {

        if (in_block_comment) {
            size_t end = s.find("*/", i);
            if (end == std::string::npos) return out; 
            in_block_comment = false;
            i = end + 2;
            continue;
        }

        if (i + 1 < n && s[i] == '/' && s[i + 1] == '/') break;
        if (i + 1 < n && s[i] == '/' && s[i + 1] == '*') {
            in_block_comment = true;
            i += 2;
            continue;
        }

        skip_ws();
        if (i >= n) break;

        const size_t start = i;
        char c = s[i];

        if (c == '"' || c == '\'') {
            char q = c; ++i;
            while (i < n) {
                char d = s[i++];
                if (d == '\\') { if (i < n) ++i; }
                else if (d == q) break;
            }
            push(start, i);
            continue;
        }

        if (std::isdigit((unsigned char)c) || (c == '.' && i + 1 < n && std::isdigit((unsigned char)s[i + 1]))) {
            size_t j = scan_number(s, i);
            if (j != i) { push(i, j); i = j; continue; }
        }

        {
            std::string op3;
            if (match3(s, i, op3)) { push(i, i + 3); i += 3; continue; }
        }
        {
            std::string op2;
            if (match2(s, i, op2)) { push(i, i + 2); i += 2; continue; }
        }

        if (is_single_delim(c)) { push(i, i + 1); ++i; continue; }

        if (is_ident_start(c)) {
            size_t j = i + 1;
            while (j < n && is_ident_cont(s[j])) ++j;
            push(i, j);
            i = j;
            continue;
        }

        size_t j = i + 1;
        while (j < n && !std::isspace((unsigned char)s[j]) && !is_single_delim(s[j])) {

            std::string dummy;
            if (match3(s, j, dummy) || match2(s, j, dummy)) break;
            ++j;
        }
        push(i, j);
        i = j;
    }

    return out;
}
