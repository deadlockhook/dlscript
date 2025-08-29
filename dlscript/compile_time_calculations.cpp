#include "compile_time_calculations.h"
#include "operation.h"
#include <iostream>


bool calculate_compile_time_expression(word_span* tokens, int token_count, var_type type, __int64& value, double& db_value, std::string& str_value)
{
	if (token_count == 0)
		return false;

	bool is_single_token = token_count == 1;

	if (token_count == 2)
	{
		auto& last_token = tokens[1];
		if (is_end_operation(last_token.text))
		{
			is_single_token = true;
		}
	}

	if (is_single_token)
	{
		auto& token = tokens[0];

		if (type == var_type_int64 || type == var_type_uint64)
		{
			if (!is_const_integer_value(token.text, value))
			{
				std::cout << "Expected integer constant, got: " << token.text << "\n";
				return false;
			}

			std::cout << "Parsed integer constant: " << value << "\n";
		}
		else if (type == var_type_float64)
		{
			if (!is_const_floating_value(token.text, db_value))
			{
				std::cout << "Expected floating point constant, got: " << token.text << "\n";
				return false;
			}

			std::cout << "Parsed floating point constant: " << db_value << "\n";
		}
		else if (type == var_type_string)
		{
			if (!is_const_string_value(token.text, str_value))
			{
				std::cout << "Expected string constant, got: " << token.text << "\n";
				return false;
			}

			std::cout << "Parsed string constant: " << str_value << "\n";
		}
		else
		{
			std::cout << "Unsupported type for compile time calculation\n";
			return false;
		}

		return true;
	}

	int current_token_index = 0;

	while (current_token_index + 1 < token_count)
	{
		auto& token = tokens[current_token_index];
		auto& operation = tokens[current_token_index + 1];

		if (type == var_type_string)
		{
			std::string op_str_value;
			if (!is_const_string_value(token.text, op_str_value))
			{
				std::cout << "Expected string constant, got: " << token.text << "\n";
				return false;
			}

			str_value += op_str_value;
		}
	}


	return true;
}