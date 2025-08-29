#pragma once
#include <string>
#include "helper_functions.h"
#include "data_types.h"

static inline bool is_paren(const std::string& t) { return t == "(" || t == ")"; }

static inline bool is_unary_candidate(const std::string& t) {
	return t == "+" || t == "-" || t == "~";
}

static inline bool is_binary_op(const std::string& t) {
	return t == "+" || t == "-" || t == "*" || t == "/" || t == "%" ||
		t == "<<" || t == ">>" || t == "&" || t == "^" || t == "|";
}

static inline int precedence(const std::string& t) {
	if (t == "*" || t == "/" || t == "%") return 5;
	if (t == "+" || t == "-")           return 4;
	if (t == "<<" || t == ">>")          return 3;
	if (t == "&")                     return 2;
	if (t == "^")                     return 1;
	if (t == "|")                     return 0;
	return -1;
}

static inline bool right_assoc(const std::string& t) {
	return false;
}

bool calculate_compile_time_expression(word_span* tokens, int token_count,var_type type, __int64& value, double& db_value, std::string& str_value);