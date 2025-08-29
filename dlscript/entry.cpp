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
#include "data_types.h"
#include "operation.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif

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
	std::vector<var_t> globals;
	std::vector<function_t> functions;
	std::vector<operation_t> start_instructions;
};

struct execution_context_t {
	void* stack_base;
	void* current_stack;
	size_t stack_size;
	std::vector<var_t> locals;
};

struct script_context_t {
	std::vector<mem_t> allocations;
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

	std::string trimmed;
	size_t start = line.find_first_not_of(" \t");
	
	if (start == std::string::npos) {
		std::cout << "Empty line\n";
		return true;
	}

	std::string first_word = line.substr(start, line.find_first_of(" \t", start) - start);

	std::cout << "First word: " << first_word << "\n";

	return true;
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

		std::cout << "Read line: " << line << "\n";
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



