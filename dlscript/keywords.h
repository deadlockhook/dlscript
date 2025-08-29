#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>

#define kw_function "function"
#define kw_true "true"
#define kw_false "false"
#define kw_nullptr "nullptr"
#define kw_null "null"

enum keyword_type : uint8_t
{
	keyword_type_function,
	keyword_type_true,
	keyword_type_false,
	keyword_type_nullptr,
	keyword_type_null,
	keyword_type_unknown
};

