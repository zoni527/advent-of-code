#include <stdio.h>
#include <string.h>
#include "libjv.h"

#define BUFLEN 64

int main(void)
{
	FILE	*file 				= NULL;
	char	buf[BUFLEN] 		= {0};
	u8		lights[999][999]	= {0}; // [y][x]
	String	line 				= {0};

	if (!(file = fopen("./06_input.txt", "r"))) {
		fprintf(stderr, "ERROR: couldn't open input file\n");
		return 1;
	}

	sReserve(&line, BUFLEN);

	String toggle = stackString("toggle ");
	String on = stackString("turn on ");
	String off = stackString("turn off ");

	while (fgets(buf, BUFLEN, file)) {
		memcpy(line.buf, buf, BUFLEN);
		line.len = strlen(buf);
		print(&line);

		if (sCompareN(&line, &toggle, 6) == 0)
			printLine(&stackString("TOGGLE"));
		if (sCompareN(&line, &on, 8) == 0)
			printLine(&stackString("ON"));
		if (sCompareN(&line, &off, 9) == 0)
			printLine(&stackString("OFF"));
	}

	sFreeBuf(&line);
	fclose(file);

	return 0;
}
