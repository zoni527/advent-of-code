#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUFLEN 16

int main(void)
{
	FILE			*file;
	char			buf[BUFLEN];
	char			*str;
	unsigned int	l, w, h, area, total, smallest;

	area = total = smallest = 0;

	if (!(file = fopen("./02_input.txt", "r"))) {
		fprintf(stderr, "Error: couldn't open input file\n");
		return 1;
	}

	while(fgets(buf, BUFLEN, file)) {
		l = atoi(buf);
		str = strchr(buf, 'x') + 1;
		w = atoi(str);
		str = strchr(str, 'x') + 1;
		h = atoi(str);

		smallest = area = l * w;
		total += 2 * area;

		area = l * h;
		if (area < smallest)
			smallest = area;
		total += 2 * area;

		area = w * h;
		if (area < smallest)
			smallest = area;
		total += 2 * area + smallest;
	}

	printf("Total required wrapping paper: %u square feet\n", total);

	return 0;
}
