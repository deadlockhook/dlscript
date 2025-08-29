#include "types.h"

type_of_token identify_token(const std::string& token, var_type& out_const_type, constant_t& out_const_value, string_constant_t& out_string_const, op_type& out_op_type, keyword_type& out_kw_type)
{
	if (is_const_integer_value(token, out_const_value.int64_value)) {
		out_const_type = var_type_int64;
		return token_type_constant;
	}

	double float_value = 0.0;
	if (is_const_floating_value(token, float_value)) {
		out_const_type = var_type_float64;
		out_const_value.float64_value = float_value;
		return token_type_constant;
	}

	if (is_const_string_value(token, out_string_const.value)) {
		out_const_type = var_type_string;
		return token_type_string_constant;
	}

	op_type op = is_operation(token);

	if (op != op_type_unknown) {
		out_op_type = op;
		return token_type_operator;
	}

	keyword_type kw = is_keyword(token);

	if (kw != keyword_type_unknown) {
		out_kw_type = kw;
		return token_type_keyword;
	}

	var_type dt = is_datatype(token);

	if (dt != var_type_unknown) {
		out_const_type = dt;
		return token_type_keyword;
	}

	return token_type_unknown;
}