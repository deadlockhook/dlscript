#include "operation.h"

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

	return op_type_unknown;
}
