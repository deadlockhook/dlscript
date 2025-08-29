#include "operation.h"

std::string op_type_to_string(op_type op)
{
	switch (op)
	{
	case op_type_end: return op_end;
	case op_type_add: return op_add;
	case op_type_sub: return op_sub;
	case op_type_mul: return op_mul;
	case op_type_div: return op_div;
	case op_type_mod: return op_mod;
	case op_type_assign: return op_assign;
	case op_type_eq: return op_eq;
	case op_type_neq: return op_neq;
	case op_type_gt: return op_gt;
	case op_type_gte: return op_gte;
	case op_type_lt: return op_lt;
	case op_type_lte: return op_lte;
	case op_type_and: return op_and;
	case op_type_or: return op_or;
	case op_type_not: return op_not;
	case op_type_inc: return op_inc;
	case op_type_dec: return op_dec;
	case op_type_bit_and: return bitop_and;
	case op_type_bit_or: return bitop_or;
	case op_type_bit_xor: return bitop_xor;
	case op_type_bit_not: return bitop_not;
	case op_type_bit_shl: return bitop_shl;
	case op_type_bit_shr: return bitop_shr;
	default: return "unknown";
	}
}

bool is_end_operation(const std::string& op)
{
	return (op == op_end);
}

op_type is_math_operation(const std::string& op) {
	if (op == op_add) return op_type_add;
	if (op == op_sub) return op_type_sub;
	if (op == op_mul) return op_type_mul;
	if (op == op_div) return op_type_div;
	if (op == op_mod) return op_type_mod;
	if (op == op_assign) return op_type_assign;
	if (op == op_eq) return op_type_eq;
	if (op == op_neq) return op_type_neq;
	if (op == op_gt) return op_type_gt;
	if (op == op_gte) return op_type_gte;
	if (op == op_lt) return op_type_lt;
	if (op == op_lte) return op_type_lte;
	if (op == op_and) return op_type_and;
	if (op == op_or) return op_type_or;
	if (op == op_not) return op_type_not;
	if (op == op_inc) return op_type_inc;
	if (op == op_dec) return op_type_dec;
	return op_type_unknown;
}

op_type is_bit_operation_type(const std::string& op) {
	if (op == bitop_and) return op_type_bit_and;
	if (op == bitop_or) return op_type_bit_or;
	if (op == bitop_xor) return op_type_bit_xor;
	if (op == bitop_not) return op_type_bit_not;
	if (op == bitop_shl) return op_type_bit_shl;
	if (op == bitop_shr) return op_type_bit_shr;
	return op_type_unknown;
}

op_type is_operation(const std::string& op) {

	op_type math_op = is_math_operation(op);

	if (math_op != op_type_unknown)
		return math_op;

	op_type bit_op = is_bit_operation_type(op);

	if (bit_op != op_type_unknown)
		return bit_op;

	if (is_end_operation(op))
		return op_type_end;

	return op_type_unknown;
}
