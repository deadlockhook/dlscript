#pragma once
#include <Windows.h>
#include <string>

#define type_int64 "__int64"
#define type_uint64 "unsigned __int64"
#define type_float64 "double"
#define type_string "string"
#define type_void "void"

enum var_type : uint8_t
{
	var_type_int64,
	var_type_uint64,
	var_type_float64,
	var_type_string,
	var_type_void,
	var_type_unknown
};

std::string type_to_string(var_type type);
var_type is_datatype(const std::string& type_str);

struct var_t
{
	std::string name;
	int type;
	void* data;
	int refcount;
};

struct constant_t
{
	var_type type;
	union {
		__int64 int64_value;
		__int32 int32_value;
		__int16 int16_value;
		__int8 int8_value;
		double float64_value;
	};

	constant_t() : type(var_type_unknown), int64_value(0) {}
};

struct string_constant_t
{
	std::string value;
};

bool is_const_integer_value(const std::string& tok, int64_t& value);
bool is_const_floating_value(const std::string& tok, double& value);
bool is_const_string_value(const std::string& tok, std::string& value);

static inline  bool is_const_integer_value(const std::string& s) { int64_t v; return is_const_integer_value(s, v); }
static inline  bool is_const_floating_value(const std::string& s) { double v; return is_const_floating_value(s, v); }
static inline  bool is_const_string_value(const std::string& s) { std::string v; return is_const_string_value(s, v); }


