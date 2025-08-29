#include <Windows.h>
#include <vector>
#include <string>

struct mem_t
{
	void* base;
	size_t size;
};

enum var_type : uint8_t
{
	type_char,
	type_int,
	type_float,
	type_double,
	type_pointer,
	type_string,
	type_raw
};

struct var_t
{
	std::string name;
	int type;
	union {
		char c;
		int i;
		float f;
		double d;
		void* p;
		std::string string;
		std::vector<uint8_t> raw;
	};
	int refcount;
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

int main() {
	
	std::string example_script = R"(
		void main() {
			int a = 5;
			int b = 10;
			int c = a + b;
			print(c);
		}
		)";

	system("pause");
	return 0;
}