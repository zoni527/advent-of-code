#include <cctype>
#include <cstdint>
#include <fstream>
#include <unordered_map>
#include <string>
#include <iostream>
#include <sstream>

enum class Op {
	Unknown,
	Assign,
	And,
	Or,
	Not,
	LShift,
	RShift,
};

struct Instruction {
	Op			op				= Op::Unknown;
	std::string	in1;
	std::string	in2;
	bool		in1_is_const	= false;
	bool		in2_is_const	= false;
	uint16_t	in1_val			= 0;
	uint16_t	in2_val			= 0;
};

static std::unordered_map<std::string, Instruction>	instructions {};
static std::unordered_map<std::string, uint16_t>	cache;

uint16_t get_signal(std::string wire)
{
	if (wire.empty())
		return 0;

	if (cache.count(wire))
		return cache[wire];

	auto		inst	= instructions[wire];
	uint16_t	result	= 0;

	switch (inst.op) {
		case Op::Assign: {
			uint16_t	i1 = inst.in1_is_const ? inst.in1_val : get_signal(inst.in1);

			result = i1;
			break;
		}
		case Op::Not: {
			uint16_t	i1 = inst.in1_is_const ? inst.in1_val : get_signal(inst.in1);

			result = ~i1;
			break;
		}
		case Op::And: {
			uint16_t	i1 = inst.in1_is_const ? inst.in1_val : get_signal(inst.in1);
			uint16_t	i2 = inst.in2_is_const ? inst.in2_val : get_signal(inst.in2);

			result = i1 & i2;
			break;
		}
		case Op::Or: {
			uint16_t	i1 = inst.in1_is_const ? inst.in1_val : get_signal(inst.in1);
			uint16_t	i2 = inst.in2_is_const ? inst.in2_val : get_signal(inst.in2);

			result = i1 | i2;
			break;
		}
		case Op::LShift: {
			uint16_t	i1 = inst.in1_is_const ? inst.in1_val : get_signal(inst.in1);
			uint16_t	i2 = inst.in2_is_const ? inst.in2_val : get_signal(inst.in2);

			result = i1 << i2;
			break;
		}
		case Op::RShift: {
			uint16_t	i1 = inst.in1_is_const ? inst.in1_val : get_signal(inst.in1);
			uint16_t	i2 = inst.in2_is_const ? inst.in2_val : get_signal(inst.in2);

			result = i1 >> i2;
			break;
		}
		default:
			std::cerr << "ERROR: unkown operation\n";
			exit(1);
	}

	cache[wire] = result;

	return result;
}

int main()
{
	std::ifstream	file("./07_input.txt");
	std::string		line {};

	if (!file) {
		std::cerr << "ERROR: couldn't open file\n";
		return 1;
	}

	while (std::getline(file, line)) {
		Instruction			inst;
		std::string			word {};
		std::stringstream	line_stream {line};
		std::string			output_wire {};

		while (line_stream >> word) {
			if (std::islower(word[0]) || std::isdigit(word[0])) {
				if (inst.op == Op::Not) {
					if (!inst.in1.empty())
						output_wire = word;
					else
						inst.in1 = word;
				}
				else if (inst.in1.empty())
					inst.in1 = word;
				else if (inst.op == Op::Assign)
					output_wire = word;
				else if (inst.in2.empty())
					inst.in2 = word;
				else if (output_wire.empty())
					output_wire = word;
				continue;
			}

			if (word == "->") {
				if (inst.op == Op::Unknown)
					inst.op = Op::Assign;
				continue;
			}

			if (word == "AND")
				inst.op = Op::And;
			else if (word == "OR")
				inst.op = Op::Or;
			else if (word == "NOT")
				inst.op = Op::Not;
			else if (word == "LSHIFT")
				inst.op = Op::LShift;
			else if (word == "RSHIFT")
				inst.op = Op::RShift;
		}

		if (std::isdigit(inst.in1[0])) {
			inst.in1_is_const = true;
			inst.in1_val = std::stoul(inst.in1);
		}

		if (std::isdigit(inst.in2[0])) {
			inst.in2_is_const = true;
			inst.in2_val = std::stoul(inst.in2);
		}

		instructions[output_wire] = inst;
	}

	std::cout << "wire 'a': " << get_signal("a") << '\n';

	return 0;
}
