#include "compile_time_eval.h"
#include "operation.h"
#include <iostream>

std::vector<std::string> normalize_unary_tokens(word_span* tokens, int n) {
    std::vector<std::string> out;
    out.reserve(n);
    enum Prev { P_NONE, P_OPERAND, P_OP, P_LPAREN } prev = P_NONE;
    for (int k = 0; k < n; k++) {
        const std::string& t = tokens[k].text;
        if (t == ";") break;

        if (t == "(") { out.push_back(t); prev = P_LPAREN; continue; }
        if (t == ")") { out.push_back(t); prev = P_OPERAND; continue; }

        if (is_unary_candidate(t)) {
            if (prev == P_NONE || prev == P_OP || prev == P_LPAREN) {
                if (t == "+") out.push_back("u+");
                else if (t == "-") out.push_back("u-");
                else out.push_back("u~");
                prev = P_OP;
                continue;
            }
        }

        out.push_back(t);
        if (is_binary_op(t) || t == "u+" || t == "u-" || t == "u~") prev = P_OP;
        else if (t == "(") prev = P_LPAREN;
        else if (t == ")") prev = P_OPERAND;
        else prev = P_OPERAND;
    }
    return out;
}

bool to_rpn(const std::vector<std::string>& toks, std::vector<rpn_item>& out, std::string& err) {
    out.clear();
    std::vector<std::string> ops;

    auto pop_while = [&](auto pred) {
        while (!ops.empty() && pred(ops.back())) { out.push_back({ true, ops.back() }); ops.pop_back(); }
        };

    for (auto& t : toks) {
        if (t == "(") { ops.push_back(t); continue; }
        if (t == ")") {
            bool matched = false;
            while (!ops.empty()) {
                if (ops.back() == "(") { matched = true; ops.pop_back(); break; }
                out.push_back({ true, ops.back() }); ops.pop_back();
            }
            if (!matched) { err = "unmatched ')'"; return false; }
            continue;
        }

        if (t == "u+" || t == "u-" || t == "u~") {
            while (!ops.empty() && ops.back() != "(" && ops.back() != "u+" && ops.back() != "u-" && ops.back() != "u~") {
                out.push_back({ true, ops.back() }); ops.pop_back();
            }
            ops.push_back(t);
            continue;
        }

        if (is_binary_op(t)) {
            int p = precedence(t);
            pop_while([&](const std::string& top) {
                if (top == "(") return false;
                if (top == "u+" || top == "u-" || top == "u~") return true;
                int pt = precedence(top);
                return pt >= p; 
                });
            ops.push_back(t);
            continue;
        }

        out.push_back({ false, t });
    }

    while (!ops.empty()) {
        if (ops.back() == "(") { err = "unmatched '('"; return false; }
        out.push_back({ true, ops.back() }); ops.pop_back();
    }
    return true;
}

bool eval_rpn_const(const std::vector<rpn_item>& rpn, const_val& out, std::string& err) {
    std::vector<const_val> st;
    for (auto& it : rpn) {
        if (!it.is_op) {
            const_val cv;
            if (!cv_from_token(it.t, cv)) { err = "non-constant token: " + it.t; return false; }
            st.push_back((cv));
            continue;
        }

        const std::string& op = it.t;

        if (op == "u+" || op == "u-" || op == "u~") {
            if (st.empty()) { err = "stack underflow (unary)"; return false; }
            const_val a = st.back(); st.pop_back();
            if (a.ty == var_type_string) { err = "unary op on string"; return false; }
            if (op == "u-") {
                if (a.ty == var_type_float64) a.f = -a.f;
                else { a.i = -a.i; a.f = (double)a.i; a.ty = var_type_int64; }
            }
            else if (op == "u~") {
                if (a.ty == var_type_float64) { err = "~ on float"; return false; }
                a.i = ~a.i; a.f = (double)a.i; a.ty = var_type_int64;
            }
            st.push_back((a));
            continue;
        }

        if (st.size() < 2) { err = "stack underflow (binary)"; return false; }
        const_val b = st.back(); st.pop_back();
        const_val a = st.back(); st.pop_back();

        if (op == "+" && (a.ty == var_type_string || b.ty == var_type_string)) {
            const_val r; r.ty = var_type_string;
            auto to_str = [](const const_val& x)->std::string {
                if (x.ty == var_type_string) return x.s;
                if (x.ty == var_type_float64) return std::to_string(x.f);
                return std::to_string(x.i);
                };
            r.s = to_str(a) + to_str(b);
            st.push_back((r));
            continue;
        }

        bool usef = (a.ty == var_type_float64) || (b.ty == var_type_float64);
        if (usef) {
            double af = (a.ty == var_type_float64) ? a.f : (double)a.i;
            double bf = (b.ty == var_type_float64) ? b.f : (double)b.i;
            const_val r; r.ty = var_type_float64;
            if (op == "+") r.f = af + bf;
            else if (op == "-") r.f = af - bf;
            else if (op == "*") r.f = af * bf;
            else if (op == "/") r.f = af / bf;
            else { err = "op not supported for floats: " + op; return false; }
            st.push_back((r));
        }
        else {
            long long ai = a.i, bi = b.i;
            const_val r; r.ty = var_type_int64;
            if (op == "+") r.i = ai + bi;
            else if (op == "-") r.i = ai - bi;
            else if (op == "*") r.i = ai * bi;
            else if (op == "/") { if (bi == 0) { err = "division by zero"; return false; } r.i = ai / bi; }
            else if (op == "%") { if (bi == 0) { err = "mod by zero"; return false; } r.i = ai % bi; }
            else if (op == "<<") r.i = ai << bi;
            else if (op == ">>") r.i = ai >> bi;
            else if (op == "&")  r.i = ai & bi;
            else if (op == "^")  r.i = ai ^ bi;
            else if (op == "|")  r.i = ai | bi;
            else { err = "unknown op: " + op; return false; }
            r.f = (double)r.i;
            st.push_back((r));
        }
    }
    if (st.size() != 1) { err = "expression did not reduce to single value"; return false; }
    out = (st.back());
    return true;
}

bool calculate_compile_time_expression(word_span* tokens, int token_count,
    var_type type, __int64& value,
    double& db_value, std::string& str_value)
{
    value = 0; db_value = 0.0; str_value.clear();
    if (token_count <= 0)
        return false;

    auto norm = normalize_unary_tokens(tokens, token_count);

    if (norm.size() == 1) {
        const_val cv;
        if (!cv_from_token(norm[0], cv)) { std::cout << "Expected constant, got: " << norm[0] << "\n"; return false; }
        if (type == var_type_string) {
            if (cv.ty == var_type_string) { str_value = cv.s; return true; }
            str_value = (cv.ty == var_type_float64) ? std::to_string(cv.f) : std::to_string(cv.i);
            return true;
        }
        if (type == var_type_float64) {
            if (cv.ty == var_type_string) { std::cout << "String not assignable to double\n"; return false; }
            db_value = (cv.ty == var_type_float64) ? cv.f : (double)cv.i;
            return true;
        }
        if (cv.ty == var_type_string) { std::cout << "String not assignable to integer\n"; return false; }
        value = (cv.ty == var_type_float64) ? (long long)cv.f : cv.i;
        return true;
    }

    std::vector<rpn_item> rpn;
    std::string err;
    if (!to_rpn(norm, rpn, err)) {
        std::cout << "Expression error: " << err << "\n";
        return false;
    }

    const_val folded;
    if (!eval_rpn_const(rpn, folded, err)) {
        std::cout << "Not a pure constant expression: " << err << "\n";
        return false;
    }

    if (type == var_type_string) {
        if (folded.ty == var_type_string) str_value = (folded.s);
        else str_value = (folded.ty == var_type_float64) ? std::to_string(folded.f) : std::to_string(folded.i);
        return true;
    }
    if (type == var_type_float64) {
        if (folded.ty == var_type_string) { std::cout << "String not assignable to double\n"; return false; }
        db_value = (folded.ty == var_type_float64) ? folded.f : (double)folded.i;
        return true;
    }

    if (folded.ty == var_type_string) { std::cout << "String not assignable to integer\n"; return false; }
    value = (folded.ty == var_type_float64) ? (long long)folded.f : folded.i;
    return true;
}

