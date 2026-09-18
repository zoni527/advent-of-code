#include "libjv.h"
#include <stdio.h>
#include <unistd.h>
#include <stdio.h>

#define BUFLEN 64


typedef void (*LightFunc)(u32*);


void lightToggle(u32 *light)
{
	*light += 2;
}


void lightOn(u32 *light)
{
	*light += 1;
}


void lightOff(u32 *light)
{
	if (*light > 0)
		*light -= 1;
}


int main(void)
{
	FILE	*file = NULL;

	if (!(file = fopen("./06_input.txt", "r"))) {
		fprintf(stderr, "ERROR: couldn't open input file\n");
		return 1;
	}

	static u32	lights[1000][1000]	= {0}; // [y][x]
	String		line 				= {0};

	sReserve(&line, BUFLEN);

	String	toggle_str	= stackString("toggle ");
	String 	on_str		= stackString("turn on ");
	String 	off_str		= stackString("turn off ");

	char	buf[BUFLEN] = {0};

	StringView	parsing_view	= {0};
	String		num_string		= {0};
	LightFunc	operation		= NULL;

	sReserve(&num_string, 9);

	while (fgets(buf, BUFLEN, file)) {
		sCopyCStringTo(buf, &line);

		operation = NULL;
		// Identify command type
		if (sCompareN(&line, &toggle_str, 6) == 0) {
			operation = lightToggle;
			parsing_view = svSliceFrom(sView(&line), 6);
		}
		else if (sCompareN(&line, &on_str, 8) == 0) {
			operation = lightOn;
			parsing_view = svSliceFrom(sView(&line), 8);
		}
		else if (sCompareN(&line, &off_str, 9) == 0) {
			operation = lightOff;
			parsing_view = svSliceFrom(sView(&line), 9);
		}
		if (operation == NULL)
			continue;

		i32		parsed_nums[4]	= {0};
		size_t	parsed_nums_idx	= 0;

		while (parsing_view.len > 0) {
			parsing_view = svSliceFrom(parsing_view, svNumberStartIdx(parsing_view));
			if (!svToI32(parsing_view, &parsed_nums[parsed_nums_idx]))
				break;
			parsing_view = svSliceFrom(parsing_view, svNumberEndIdx(parsing_view));
			++parsed_nums_idx;
		}

		for (i32 j = parsed_nums[1]; j <= parsed_nums[3]; ++j) {
			for (i32 i = parsed_nums[0]; i <= parsed_nums[2]; ++i) {
				operation(&lights[j][i]);
			}
		}
	}

	sFreeBuf(&line);
	fclose(file);

	i64	total_brightness = 0;

	for (i32 j = 0; j < 1000; ++j) {
		for (i32 i = 0; i < 1000; ++i) {
			total_brightness += lights[j][i];
		}
	}

	i64IntoString(total_brightness, &num_string);
	printLine(&num_string);
	sFreeBuf(&num_string);

	return 0;
}
