#include <vector>
#include <string>
#include <cctype>
#include <cstring>
#include <limits>
#include <string>
#include "tokenizer.h"
#include "data_types.h"
#include <iostream>
#include "operation.h"

struct rpn_item { bool is_op; std::string t; };

std::vector<std::string> normalize_unary_tokens(word_span* tokens, int n);

bool to_rpn(const std::vector<std::string>& toks, std::vector<rpn_item>& out, std::string& err);

bool eval_rpn_const(const std::vector<rpn_item>& rpn, const_val& out, std::string& err);

bool calculate_compile_time_expression(word_span* tokens, int token_count,
    var_type type, __int64& value,
    double& db_value, std::string& str_value);
