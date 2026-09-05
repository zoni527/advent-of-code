#include <iostream>
#include <string>
#include <fstream>

int main()
{
	std::ifstream	file{"./01_input.txt"};
	std::string		input;
	int				floor{0};
	unsigned		count{0};

	if (file.bad()) {
		std::cerr << "Error: couldn't open input file\n";
		return 1;
	}

	while (std::getline(file, input)) {
		for (auto const &c : input) {
			++count;
			switch (c) {
				case '(': ++floor; break;
				case ')':
					--floor;
					if (floor == -1) {
						std::cout << "Santa has entered the basement at position " << count << "\n";
						return 0;
					}
				break;
				default:
					std::cerr << "Invalid input character\n";
					return 1;
			}
		}
	}

	std::cout << "Santa needs to go to floor " << floor << "\n";

	return 0;
}
