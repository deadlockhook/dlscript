#pragma once

#include "data_types.h"
#include "operation.h"
#include "keywords.h"

enum type_of_token : uint8_t
{
	token_type_unknown,
	token_type_constant,
	token_type_string_constant,
	token_type_operator,
	token_type_keyword
};

type_of_token identify_token(const std::string& token, var_type& out_const_type, constant_t& out_const_value, string_constant_t& out_string_const, op_type& out_op_type, keyword_type& out_kw_type);