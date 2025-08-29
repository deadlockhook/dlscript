#include <Windows.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <charconv>
#include <limits>
#include <cctype>
#include <cstdlib>
#include <cerrno>
#include <cstdint>
#include <system_error> 
#include <iomanip> 
#include "helper_functions.h"
#include "types.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <map>

struct mem_t
{
	void* base;
	size_t size;
};

enum instruction_type : uint8_t
{
	instr_noop,
	instr_push,
	instr_pop,
	instr_add,
	instr_sub,
	instr_mul,
	instr_div,
	instr_mod,
	instr_jmp,
	instr_jz,
	instr_jnz,
	instr_call,
	instr_ret
};

struct operation_t
{
	instruction_type instruction_type;
	uint8_t instruction_bytes[15];
};

struct function_t {
	std::string name;
	std::vector<operation_t> instructions;
};

struct compiled_script_t {
	/*use hashing instead*/
	std::map<std::string, var_t> globals;
	std::vector<function_t> functions;
	std::vector<operation_t> start_instructions;
	
	bool has_global_already(const std::string& name) {
		return globals.find(name) != globals.end();
	}
};

struct execution_context_t {
	void* stack_base;
	void* current_stack;
	size_t stack_size;
	std::vector<var_t> locals;
};

struct script_context_t {
	std::map<void*, mem_t> allocations;
	std::vector<execution_context_t> exec_contexts;
	compiled_script_t compiled_data;
};

#define long_comment_start "/*"
#define long_comment_end "*/"
#define short_comment "//"

/*all of this can be hashed for quicker compilation */

bool is_comment(const std::string& token) {
	return (token == long_comment_start || token == long_comment_end || token == short_comment);
}



bool is_const_value(const std::string& token)
{
	return is_const_floating_value(token)
		|| is_const_integer_value(token)
		|| is_const_string_value(token);
}

bool parse_line(const std::string& line, script_context_t& ctx,bool& in_function_context)
{
	/*find next non whitespace*/
	auto ws = words_ws(line);
	size_t w_count = ws.size();
	if (!w_count)
	{
		std::cout << "Empty line\n";
		return true;
	}

	std::string trimmed;
	size_t start = line.find_first_not_of(" \t");
	
	if (start == std::string::npos) {
		std::cout << "Empty line\n";
		return true;
	}

	auto& first_word = ws[0];
	std::cout << "first_word: " << first_word.text << "\n";

	type_token_t first_token_info;
	type_of_token first_token_type = identify_token(first_word.text.data(), first_token_info);

	if (first_token_type != token_type_datatype && first_token_type != token_type_keyword && first_token_type != token_type_identifier) {
		std::cout << "Invalid start of line: " << first_word.text << "\n";
		return false;
	}

	if (first_token_type == token_type_datatype)
	{
		if (w_count < 2)
		{
			std::cout << "Expected variable name after type\n";
			return false;
		}



		auto& next_word= ws[1];
	
		if (next_word.text.empty()) {
			std::cout << "Expected variable name after type\n";
			return false;
		}

		std::cout << "next_word: " << next_word.text << "\n";

		type_token_t second_token_info;
		type_of_token second_token_type = identify_token(next_word.text, second_token_info);

		if (second_token_type == token_type_keyword)
		{
			if (second_token_info.out_kw_type == keyword_type_function) {

				if (in_function_context)
				{
					std::cout << "Nested functions not supported yet\n";
					return false;
				}

				std::cout << "Function declaration not supported yet\n";
				//in_function_context = true;
				//parse function declaration
				return false;
			}
			else {
				std::cout << "Unexpected keyword after type: " << next_word.text << "\n";
				return false;
			}
		}

		if (second_token_type != token_type_identifier) {
			std::cout << "Expected variable name after type, got: " << token_type_to_string(second_token_type) << "\n";
			return false;
		}

		if (first_token_info.const_type == var_type_void) {
			std::cout << "Variable cannot be of type void\n";
			return false;
		}

		if (in_function_context) {
			std::cout << "Variable declaration inside function not supported yet\n";
			return false;
		}
		else
		{
			if (ctx.compiled_data.has_global_already(next_word.text))
			{
				std::cout << "Global variable already declared: " << next_word.text << "\n";
				return false;
			}
		}

		bool has_assignment = false;
	
		if (w_count >= 3)
		{
			auto& third_word = ws[2];
		
			if (third_word.text == "=")
				has_assignment = true;
		}

		int64_t int64_value = 0;
		double floating_value = 0.0;
		std::string string_value;
		var_type assigned_type = var_type_unknown;

		if (has_assignment)
		{
			if (w_count < 4)
			{
				std::cout << "Expected value after '='\n";
				return false;
			}

			/*for now i will support only assignment of one value, later i will support calculations*/
			/*we will also optimize by calculating these values before hand*/

			if (first_token_info.const_type == var_type_int64 || first_token_info.const_type == var_type_int32
				|| first_token_info.const_type == var_type_int16
				|| first_token_info.const_type == var_type_int8
				|| first_token_info.const_type == var_type_uint64
				|| first_token_info.const_type == var_type_uint32
				|| first_token_info.const_type == var_type_uint16
				|| first_token_info.const_type == var_type_uint8)
			{
				auto& value_word = ws[3];
		
				if (!is_const_integer_value(value_word.text, int64_value))
				{
					std::cout << "Expected integer constant after '=', got: " << value_word.text << "\n";
					return false;
				}

				assigned_type = var_type_int64;
			}
			else if (first_token_info.const_type == var_type_float64)
			{
				auto& value_word = ws[3];
			
				if (!is_const_floating_value(value_word.text, floating_value))
				{
					std::cout << "Expected floating point constant after '=', got: " << value_word.text << "\n";
					return false;
				}

				assigned_type = var_type_float64;
			}
			else if (first_token_info.const_type == var_type_string)
			{

				auto& value_word = ws[3];

				if (!is_const_string_value(value_word.text, string_value))
				{
					std::cout << "Expected string constant after '=', got: " << value_word.text << "\n";
					return false;
				}

				assigned_type = var_type_string;
			}
			else
			{
				std::cout << "Unsupported variable type for assignment\n";
				return false;
			}
		}

		switch (first_token_info.const_type)
		{
		case var_type_int64:
		{
			var_t v;
			v.name = next_word.text;
			v.type = var_type_int64;
			v.data = new __int64(0);
			v.refcount = 1;
			ctx.allocations[v.data] = mem_t{ v.data, sizeof(__int64) };
			ctx.compiled_data.globals[v.name] = v;
			break;
		}
		case var_type_int32:
		{
			var_t v;
			v.name = next_word.text;
			v.type = var_type_int32;
			v.data = new __int32(0);
			v.refcount = 1;
			ctx.allocations[v.data] = mem_t{ v.data, sizeof(__int32) };
			ctx.compiled_data.globals[v.name] = v;
			break;
		}
		case var_type_int16:
		{
			var_t v;
			v.name = next_word.text;
			v.type = var_type_int16;
			v.data = new __int16(0);
			v.refcount = 1;
			ctx.allocations[v.data] = mem_t{ v.data, sizeof(__int16) };
			ctx.compiled_data.globals[v.name] = v;
			break;
		}
		case var_type_int8:
		{
			var_t v;
			v.name = next_word.text;
			v.type = var_type_int8;
			v.data = new __int8(0);
			v.refcount = 1;
			ctx.allocations[v.data] = mem_t{ v.data, sizeof(__int8) };
			ctx.compiled_data.globals[v.name] = v;
			break;
		}
		case var_type_uint64:
		{
			var_t v;
			v.name = next_word.text;
			v.type = var_type_uint64;
			v.data = new unsigned __int64(0);
			v.refcount = 1;
			ctx.allocations[v.data] = mem_t{ v.data, sizeof(unsigned __int64) };
			ctx.compiled_data.globals[v.name] = v;
			break;
		}
		case var_type_uint32:
		{
			var_t v;
			v.name = next_word.text;
			v.type = var_type_uint32;
			v.data = new unsigned __int32(0);
			v.refcount = 1;
			ctx.allocations[v.data] = mem_t{ v.data, sizeof(unsigned __int32) };
			ctx.compiled_data.globals[v.name] = v;
			break;
		}
		case var_type_uint16:
		{
			var_t v;
			v.name = next_word.text;
			v.type = var_type_uint16;
			v.data = new unsigned __int16(0);
			v.refcount = 1;
			ctx.allocations[v.data] = mem_t{ v.data, sizeof(unsigned __int16) };
			ctx.compiled_data.globals[v.name] = v;
			break;
		}
		case var_type_uint8:
		{
			var_t v;
			v.name = next_word.text;
			v.type = var_type_uint8;
			v.data = new unsigned __int8(0);
			v.refcount = 1;
			ctx.allocations[v.data] = mem_t{ v.data, sizeof(unsigned __int8) };
			ctx.compiled_data.globals[v.name] = v;
			break;
		}
		case var_type_float64:
		{
			var_t v;
			v.name = next_word.text;
			v.type = var_type_float64;
			v.data = new double(0.0);
			v.refcount = 1;
			ctx.allocations[v.data] = mem_t{ v.data, sizeof(double) };
			ctx.compiled_data.globals[v.name] = v;
			break;
		}
		case var_type_string:
		{
			var_t v;
			v.name = next_word.text;
			v.type = var_type_string;
			v.data = new std::string("");
			v.refcount = 1;
			ctx.allocations[v.data] = mem_t{ v.data, sizeof(std::string) };
			ctx.compiled_data.globals[v.name] = v;
			break;
		}
		default:
			std::cout << "Unsupported variable type\n";
			return false;
		}


		return true;
	}


	std::cout << "failed to parse line: " << line << "\n";

	return false;
}

bool parse_script(const std::string& script, script_context_t& ctx) {
	

	std::istringstream in(script);
	std::string line;
	bool in_function_context = false;
	while (std::getline(in, line)) {

		if (!line.empty() && line.back() == '\r') 
			line.pop_back();

		if (!parse_line(line, ctx, in_function_context))
			return false;

		//std::cout << "Read line: " << line << "\n";
	}

	return true;
}

int main() {
	
	std::string example_script = R"(
		__int64 global_var = 0;
		)";

	script_context_t ctx;
	std::cout << parse_script(example_script, ctx)<< "\n";

	system("pause");
	return 0;
}



