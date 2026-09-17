#include "libjv.h"
#include <stdint.h>
#include <stdbool.h>

static char	*sBuffer(size_t size);


ssize_t	print(const String *str)
{
	return write(STDOUT_FILENO, str->buf, str->len);
}


# define PRINTLINE_LEN_MAX 512

ssize_t printLine(const String *str)
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


static char *sBuffer(size_t size)
{
	char	*buf = calloc(size, sizeof(*buf));

	if (!buf) {
		fprintf(stderr, "ERROR: couldn't allocate string buffer\n");
		exit(1);
	}

	return buf;
}


String *sEmpty(void)
{
	String	*empty_string = calloc(1, sizeof(*empty_string));

	if (!empty_string) {
		fprintf(stderr, "ERROR: couldn't allocate string buffer\n");
		exit(1);
	}

	return empty_string;
}


String *sCopy(const String *str)
{
	String	*copy = sEmpty();

	copy->len = str->len;
	copy->cap = str->cap;

	// If string is on the stack cap is marked as 0, so use length
	if (!copy->cap)
		copy->cap = copy->len;
	copy->buf = sBuffer(str->cap);
	memcpy(copy->buf, str->buf, str->len);

	return copy;
}


String *sJoin(const String *s1, const String *s2)
{
	String	*joined = sEmpty();

	joined->len = s1->len + s2->len;
	joined->cap = joined->len;

	joined->buf = sBuffer(joined->len);
	memcpy(joined->buf, s1->buf, s1->len);
	memcpy(joined->buf + s1->len, s2->buf, s2->len);

	return joined;
}


String *sAppend(String *s1, const String *s2)
{
	if (s1->cap - s1->len < s2->len) {
		char	*str_buf = sBuffer(s1->len + s2->len);

		memcpy(str_buf, s1->buf, s1->len);
		free(s1->buf);
		s1->buf = str_buf;
	}

	memcpy(s1->buf + s1->len, s2->buf, s2->len);
	s1->len += s2->len;

	return s1;
}


bool stringToI32(const String *str, i32 *out)
{
	if (!out)
		return false;

	*out = INT32_MIN;

	if (!str || str->len == 0)
		return false;

	size_t	i = 0;

	// Whitespace skip
	while (i < str->len &&
			(str->buf[i] == ' ' || (str->buf[i] >= 9 && str->buf[i] <= 13)))
		++i;

	if (i >= str->len)
		return false;

	i32	sign = 1;

	if (str->buf[i] == '-' || str->buf[i] == '+') {
		if (str->buf[i] == '-')
			sign = -1;
		++i;
	}

	// Only sign character without digits
	if (i == str->len || str->buf[i] < '0' || str->buf[i] > '9')
		return false;

	i32	num = 0;

	while (i < str->len && (str->buf[i] >= '0' && str->buf[i] <= '9')) {
		i32	digit = str->buf[i] - '0';

		if (sign > 0) {
			if (num > (INT32_MAX - digit) / 10) {
				*out = INT32_MAX;
				return false;
			}
			num = 10 * num + digit;
		}
		if (sign < 0) {
			if (num < (INT32_MIN + digit) / 10)
				return false;
			num = 10 * num - digit;
		}
		++i;
	}

	*out = num;

	return true;
}


/**
 * WARNING: If used with string that already has a buffer
 *          will truncate that buffer if size is less than
 *          the buffer's length
 */
String *sReserve(String *str, size_t size)
{
	char	*buf = sBuffer(size);
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


void sFreeBuf(String *str)
{
	if (!str)
		return;

	free(str->buf);
	str->buf = NULL;
}

void sFreePtr(String **ptr)
{
	if (!ptr)
		return;

	sFreeBuf(*ptr);
	*ptr = NULL;
}


StringView sView(const String *str)
{
	return (StringView){.len = str->len, .buf = str->buf};
}


StringView sSlice(const String *str, i32 start, i32 end)
{
	size_t	start_idx;

	if (start < 0 && end < 0)
		return (StringView){0};

	return (StringView){.buf = str->buf + start, .len = end - start};
}


i32 sCompare(const String *s1, const String *s2)
{
	size_t	min_len = (s1->len < s2->len) ? s1->len : s2->len;

	for (size_t i = 0; i < min_len; ++i)
		if (s1->buf[i] != s2->buf[i])
			return ((i32)(u8)s1->buf[i] - (i32)(u8)s2->buf[i]);

	if (s1->len > s2->len)
		return 1;
	if (s1->len < s2->len)
		return -1;

	return 0;
}


i32 sCompareN(const String *s1, const String *s2, size_t n)
{
	if (n == 0)
		return 0;

	size_t	i = 0;

	for (; i < n && i < s1->len && i < s2->len; ++i)
		if (s1->buf[i] != s2->buf[i])
			return ((i32)(u8)s1->buf[i] - (i32)(u8)s2->buf[i]);
	
	if (i == n)
		return 0;

	if (s1->len > s2->len)
		return 1;
	if (s1->len < s2->len)
		return -1;

	return 0;
}


String *i64ToString(i64 num)
{
	size_t	num_len = i64DecimalDigits(num);
	i8		sign = 1;

	// Minus sign
	if (num < 0) {
		++num_len;
		sign = -1;
	}

	String	*num_string = sEmpty();

	sReserve(num_string, num_len);
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


String *i64IntoString(i64 num, String *str)
{
	size_t	num_len = i64DecimalDigits(num);
	i8		sign = 1;

	if (num < 0) {
		++num_len;
		sign = -1;
	}

	if (str->cap < num_len)
		sReserve(str, num_len);

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


size_t u64DecimalDigits(u64 num)
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


size_t i64DecimalDigits(i64 num)
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
