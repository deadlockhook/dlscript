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
	token_type_keyword,
	token_type_datatype,
	token_type_identifier
};

struct type_token_t
{
	var_type const_type;
	constant_t const_value;
	string_constant_t string_value;
	op_type out_op_type;
	keyword_type out_kw_type;
	type_of_token token_type;
};

std::string token_type_to_string(type_of_token type);
type_of_token identify_token(const std::string& token, type_token_t& out);

