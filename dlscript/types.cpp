#include "types.h"
#include "helper_functions.h"

type_of_token identify_token(const std::string& token, type_token_t& out)
{
	if (is_const_integer_value(token, out.const_value.int64_value)) {
		out.const_type = var_type_int64;
		return token_type_constant;
	}

	double float_value = 0.0;
	if (is_const_floating_value(token, float_value)) {
		out.const_type = var_type_float64;
		out.const_value.float64_value = float_value;
		return token_type_constant;
	}

	if (is_const_string_value(token, out.string_value.value)) {
		out.const_type = var_type_string;
		return token_type_string_constant;
	}

	op_type op = is_operation(token);

	if (op != op_type_unknown) {
		out.out_op_type = op;
		return token_type_operator;
	}

	keyword_type kw = is_keyword(token);

	if (kw != keyword_type_unknown) {
		out.out_kw_type = kw;
		return token_type_keyword;
	}

	var_type dt = is_datatype(token);

	if (dt != var_type_unknown) {
		out.const_type = dt;
		return token_type_datatype;
	}

	if (is_identifier(token)) {
		out.token_type = token_type_identifier;   
		return token_type_identifier;
	}

	return token_type_unknown;
}

std::string token_type_to_string(type_of_token type)
{
	switch (type)
	{
	case token_type_constant: return "constant";
	case token_type_string_constant: return "string constant";
	case token_type_operator: return "operator";
	case token_type_keyword: return "keyword";
	case token_type_datatype: return "data type";
	case token_type_identifier: return "identifier";
	default: return "unknown";
	}
}