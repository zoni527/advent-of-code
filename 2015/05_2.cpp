#include <fstream>
#include <iostream>
#include <string_view>

int main()
{
	std::ifstream	file{"./day_05/src/input.txt"};

	if (!file) {
		std::cerr << "ERROR: couldn't open file\n";
		return 1;
	}

	std::string	line{};
	size_t		good_lines{0};

	while (std::getline(file, line)) {
		bool non_overlapping_pair = false;

		// Check for non overlapping pairs of letters
		for (auto it = line.begin(); it < line.end() - 1; ++it) {
			std::string_view first{&*it, 2};

			for (auto jt = it + 2; jt < line.end() - 1; ++jt) {
				std::string_view second{&*jt, 2};

				if (first == second) {
					non_overlapping_pair = true;
					break;
				}
			}
		}

		if (!non_overlapping_pair)
			continue;

		bool mirrored_group_of_3 = false;

		// Check for mirrored group of 3
		for (auto it = line.begin(); it < line.end() - 2; ++it)
			if (*it == *(it + 2)) {
				mirrored_group_of_3 = true;
				break;
			}

		if (!mirrored_group_of_3)
			continue;

		++good_lines;
	}
	
	std::cout << good_lines << "\n";

	return 0;
}
