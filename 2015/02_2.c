#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUFLEN 16

unsigned int smallest_perimeter(
	unsigned int l,
	unsigned int w,
	unsigned int h)
{
	// Length is the longest
	if (w <= l && h <= l)
		return 2 * w + 2 * h;

	// Height ist the longest
	if (l <= h && w <= h)
		return 2 * l + 2 * w;

	// Width is the longest
	return 2 * l + 2 * h;
}

int main(void)
{
	FILE			*file;
	char			buf[BUFLEN];
	char			*str;
	unsigned int	l, w, h, bow, total;

	l = w = h = bow = total = 0;

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
		printf("%u %u %u\n", l, w, h);

		total += l * w * h + smallest_perimeter(l, w, h);
	}

	printf("Total required ribbon: %u feet\n", total);

	return 0;
}
