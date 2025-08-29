#pragma once
#include <Windows.h>
#include <string>
#include "data_types.h"
#include <vector>

#define op_end ";"
#define op_add "+"
#define op_sub "-"
#define op_mul "*"
#define op_div "/"
#define op_mod "%"
#define op_assign "="
#define op_eq "=="
#define op_neq "!="
#define op_gt ">"
#define op_gte ">="
#define op_lt "<"
#define op_lte "<="
#define op_and "&&"
#define op_or "||"
#define op_not "!"
#define op_inc "++"
#define op_dec "--"

#define bitop_and "&"
#define bitop_or "|"
#define bitop_xor "^"
#define bitop_not "~"
#define bitop_shl "<<"
#define bitop_shr ">>"

enum op_type : uint8_t {
	op_type_end,
	op_type_add,
	op_type_sub,
	op_type_mul,
	op_type_div,
	op_type_mod,
	op_type_assign,
	op_type_eq,
	op_type_neq,
	op_type_gt,
	op_type_gte,
	op_type_lt,
	op_type_lte,
	op_type_and,
	op_type_or,
	op_type_not,
	op_type_inc,
	op_type_dec,
	op_type_bit_and,
	op_type_bit_or,
	op_type_bit_xor,
	op_type_bit_not,
	op_type_bit_shl,
	op_type_bit_shr,
	op_type_unknown
};

struct numeric_operation_data_t
{
	op_type type;
	union {
		int64_t int64_value;
		double float64_value;
	};
	var_type value_type;
};

struct numeric_operation_t
{
	op_type type;
	std::vector<numeric_operation_data_t*> operands;
};

std::string op_type_to_string(op_type op);

bool is_end_operation(const std::string& op);
op_type is_math_operation(const std::string& op);
op_type is_bit_operation_type(const std::string& op);
op_type is_operation(const std::string& op);