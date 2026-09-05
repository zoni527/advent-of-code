#include "libjv.h"

static char	*stringBuffer(size_t size);


ssize_t	print(const String *str)
{
	return write(STDOUT_FILENO, str->buf, str->len);
}


# define PRINTLINE_LEN_MAX 512

ssize_t	printLine(const String *str)
{
	char	buf[PRINTLINE_LEN_MAX];
	size_t	bytes;

	bytes = str->len;
	if (str->len >= PRINTLINE_LEN_MAX - 1)
		bytes = PRINTLINE_LEN_MAX - 1;

	memcpy(buf, str->buf, bytes);
	buf[bytes] = '\n';

	return write(STDOUT_FILENO, buf, bytes + 1);
}


String	*emptyString(void)
{
	String	*str = calloc(1, sizeof(*str));

	if (!str) {
		fprintf(stderr, "ERROR: couldn't allocate string\n");
		exit(1);
	}

	return str;
}


static char	*stringBuffer(size_t size)
{
	char	*buf = calloc(size, sizeof(*buf));

	if (!buf) {
		fprintf(stderr, "ERROR: couldn't allocate string buffer\n");
		exit(1);
	}

	return buf;
}


String	*stringCopy(const String *str)
{
	String	*copy = emptyString();

	copy->len = str->len;
	copy->cap = str->cap;

	// If string is on the stack cap is marked as 0, so use length
	if (!copy->cap)
		copy->cap = copy->len;
	copy->buf = stringBuffer(str->cap);
	memcpy(copy->buf, str->buf, str->len);

	return copy;
}


String	*stringJoin(const String *s1, const String *s2)
{
	String	*joined = emptyString();

	joined->len = s1->len + s2->len;
	joined->cap = joined->len;

	joined->buf = stringBuffer(joined->len);
	memcpy(joined->buf, s1->buf, s1->len);
	memcpy(joined->buf + s1->len, s2->buf, s2->len);

	return joined;
}

String	*stringConcatTo(String *s1, const String *s2)
{
	if (s1->cap - s1->len < s2->len) {
		char	*str_buf = stringBuffer(s1->len + s2->len);

		memcpy(str_buf, s1->buf, s1->len);
		free(s1->buf);
		s1->buf = str_buf;
	}

	memcpy(s1->buf + s1->len, s2->buf, s2->len);
	s1->len += s2->len;

	return s1;
}


/**
 * WARNING: If used with string that already has a buffer
 *          will truncate that buffer if size is less than
 *          the buffer's length
 */
String	*stringReserve(String *str, size_t size)
{
	char	*buf = stringBuffer(size);
	size_t	bytes = size < str->len ? size : str->len;

	if (str->buf) {
		memcpy(buf, str->buf, bytes);
		str->len = bytes;
		free(str->buf);
	}
	str->buf = buf;
	str->cap = size;

	return str;
}


void	stringFree(String **str_ptr)
{
	if (!str_ptr)
		return;

	free((*str_ptr)->buf);
	free(*str_ptr);
	*str_ptr = NULL;
}


String	*i64ToString(i64 num)
{
	size_t	num_len = i64DecimalDigits(num);
	i8		sign = 1;

	// Minus sign
	if (num < 0) {
		++num_len;
		sign = -1;
	}

	String	*num_string = emptyString();

	stringReserve(num_string, num_len);
	for (size_t i = 0; i < num_len && num != 0; ++i) {
		i8	digit = (num % 10) * sign;

		num_string->buf[num_len - 1 - i] = '0' + digit;
		num /= 10;
	}
	if (sign == -1)
		num_string->buf[0] = '-';
	num_string->len = num_len;
	
	return num_string;
}


String	*i64IntoString(i64 num, String *str)
{
	size_t	num_len = i64DecimalDigits(num);
	i8		sign = 1;

	if (num < 0) {
		++num_len;
		sign = -1;
	}

	if (str->cap < num_len)
		stringReserve(str, num_len);

	for (size_t i = 0; i < num_len && num != 0; ++i) {
		i8	digit = (num % 10) * sign;

		str->buf[num_len - 1 - i] = '0' + digit;
		num /= 10;
	}
	if (sign == -1)
		str->buf[0] = '-';
	str->len = num_len;

	return str;
}


size_t	u64DecimalDigits(u64 num)
{
	size_t	digits = 1;

	while (1) {
		num /= 10;
		if (num == 0)
			break;
		++digits;
	}

	return digits;
}


size_t	i64DecimalDigits(i64 num)
{
	size_t	digits = 1;

	while (1) {
		num /= 10;
		if (num == 0)
			break;
		++digits;
	}

	return digits;
}
