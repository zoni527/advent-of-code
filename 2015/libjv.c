#include "libjv.h"
#include <unistd.h>
#include <stdio.h>


static char	*sBuffer(size_t size);


ssize_t	print(const String *str)
{
	return write(STDOUT_FILENO, str->buf, str->len);
}


# define PRINTLINE_LEN_MAX 4096

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
	if (size == 0)
		size = 1;

	char	*buf = calloc(size, sizeof(*buf));

	if (!buf) {
		fprintf(stderr, "ERROR: couldn't allocate string buffer\n");
		return NULL;
	}

	return buf;
}


String *sEmpty(void)
{
	String	*empty_string = calloc(1, sizeof(*empty_string));

	if (!empty_string) {
		fprintf(stderr, "ERROR: couldn't allocate string buffer\n");
		return NULL;
	}

	return empty_string;
}


String *sCopyTo(const String *src, String *dest)
{
	if (!src || !dest)
		return NULL;

	if (dest->cap < src->len)
		if (!sReserve(dest, src->len))
			return NULL;

	memcpy(dest->buf, src->buf, src->len);
	dest->len = src->len;

	return dest;
}


String *sCopy(const String *str)
{
	String	*copy = sEmpty();

	if (!copy)
		return NULL;

	if (!sCopyTo(str, copy))
		return NULL;

	return copy;
}


static size_t jvStrlen(const char *str)
{
	if (!str)
		return 0;

	size_t	len = 0;

	while (*(str + len))
		++len;

	return len;
}


String *sCopyCStringTo(const char *src, String *dest)
{
	if (!src || !dest)
		return NULL;

	size_t	len = jvStrlen(src);

	if (len == 0) {
		dest->len = 0;
		return dest;
	}

	if (dest->cap < len)
		if (!sReserve(dest, len))
			return NULL;

	memcpy(dest->buf, src, len);
	dest->len = len;

	return dest;
}


String *sJoin(const String *s1, const String *s2)
{
	if (!s1 || !s2)
		return NULL;

	String	*joined = sEmpty();

	if (!joined)
		return NULL;


	if (!sReserve(joined, s1->len + s2->len)) {
		free(joined);
		return NULL;
	}

	if (!sAppend(joined, s1) || !sAppend(joined, s2)) {
		sDestroy(&joined);
		return NULL;
	}

	return joined;
}


String *sAppend(String *s1, const String *s2)
{
	if (!s1 || !s2)
		return s1;

	size_t	combined_len = s1->len + s2->len;

	size_t	growth_cap = s1->cap * 2;
	size_t	final_cap = (growth_cap > combined_len) ? growth_cap : combined_len;

	if (!sReserve(s1, final_cap))
		return NULL;

	memcpy(s1->buf + s1->len, s2->buf, s2->len);
	s1->len = combined_len;

	return s1;
}


String	*sReserve(String *str, size_t cap)
{
	if (!str)
		return NULL;

	if (str->cap >= cap)
		return str;

	char	*buf = sBuffer(cap);

	if (!buf)
		return NULL;

	if (str->buf) {
		memcpy(buf, str->buf, str->len);
		free(str->buf);
	}

	str->buf = buf;
	str->cap = cap;

	return str;
}


/**
 * WARNING: If used with string that already has a buffer
 *          will truncate that buffer if size is less than
 *          the buffer's length
 */
String *sResize(String *str, size_t len)
{
	if (!str)
		return NULL;

	if (len <= str->cap) {
		str->len = len;
		return str;
	}

	char	*buf = sBuffer(len);

	if (!buf)
		return NULL;

	size_t	bytes = (len < str->len) ? len : str->len;

	if (str->buf) {
		memcpy(buf, str->buf, bytes);
		free(str->buf);
	}

	str->buf = buf;
	str->cap = len;
	str->len = len;

	return str;
}


void sFreeBuf(String *str)
{
	if (!str)
		return;

	free(str->buf);
	str->buf = NULL;
}


void sDestroy(String **ptr)
{
	if (!ptr)
		return;

	sFreeBuf(*ptr);
	free(*ptr);
	*ptr = NULL;
}


StringView sView(const String *str)
{
	if (!str)
		return (StringView){0};

	return (StringView){.len = str->len, .buf = str->buf};
}


StringView svSlice(const StringView sv, i64 start, i64 end)
{
	i64 s = (start < 0) ? (i64)sv.len + start : (i64)start;
	i64 e = (end < 0) ? (i64)sv.len + end : (i64)end;

	// Clamping start
	if (s < 0)
		s = 0;
	if (s > (i64)sv.len)
		s = (i64)sv.len;

	// Clamping end
	if (e < 0)
		e = 0;
	if (e > (i64)sv.len)
		e = (i64)sv.len;

	// Empty slice, no elements between indices
	if (s >= e)
		return(StringView){0};

	return (StringView){
		.buf = sv.buf + (size_t)s,
		.len = (size_t)(e - s)
	};
}


StringView svSliceFrom(const StringView sv, i64 start)
{
	return svSlice(sv, start, sv.len);
}


StringView sSlice(const String *str, i64 start, i64 end)
{
	if (!str || !str->buf)
		return (StringView){0};

	return svSlice(sView(str), start, end);
}


i32 sCompare(const String *s1, const String *s2)
{
	if (!s1 || !s2)
		return INT32_MIN;

	return svCompare(sView(s1), sView(s2));
}


i32 sCompareN(const String *s1, const String *s2, size_t n)
{
	if (!s1 || !s2)
		return INT32_MIN;

	return svCompareN(sView(s1), sView(s2), n);
}


i32 svCompare(StringView sv1, StringView sv2)
{
	size_t	min_len = (sv1.len < sv2.len) ? sv1.len : sv2.len;

	for (size_t i = 0; i < min_len; ++i)
		if (sv1.buf[i] != sv2.buf[i])
			return ((i32)(u8)sv1.buf[i] - (i32)(u8)sv2.buf[i]);

	if (sv1.len > sv2.len)
		return 1;
	if (sv1.len < sv2.len)
		return -1;

	return 0;
}


i32 svCompareN(StringView sv1, StringView sv2, size_t n)
{
	if (n == 0)
		return 0;

	size_t	i = 0;
	size_t	min_len = n;

	if (sv1.len < min_len)
		min_len = sv1.len;
	if (sv2.len < min_len)
		min_len = sv2.len;

	for (; i < min_len; ++i)
		if (sv1.buf[i] != sv2.buf[i])
			return ((i32)(u8)sv1.buf[i] - (i32)(u8)sv2.buf[i]);

	if (i == n)
		return 0;

	if (sv1.len > sv2.len)
		return 1;
	if (sv1.len < sv2.len)
		return -1;

	return 0;
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
		if (!sResize(str, num_len))
			return NULL;

	if (num == 0) {
		str->buf[0] = '0';
		str->len = 1;

		return str;
	}

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


String *i64ToString(i64 num)
{
	String	*num_string = sEmpty();

	if (!num_string)
		return NULL;

	return i64IntoString(num, num_string);
}


# define IMPLEMENT_DIGITS_COUNTING_LOGIC \
	size_t	digits = 1; \
	while (1) { \
		num /= 10; \
		if (num == 0) \
			break; \
		++digits; \
	} \
	return digits;

size_t u64DecimalDigits(u64 num)
{
	IMPLEMENT_DIGITS_COUNTING_LOGIC
}


size_t i64DecimalDigits(i64 num)
{
	IMPLEMENT_DIGITS_COUNTING_LOGIC
}


bool svToI32(StringView sv, i32 *out)
{
	if (!out)
		return false;

	*out = INT32_MIN;

	if (sv.len == 0)
		return false;

	size_t	i = 0;

	// Whitespace skip
	while (i < sv.len &&
			(sv.buf[i] == ' ' || (sv.buf[i] >= 9 && sv.buf[i] <= 13)))
		++i;

	if (i >= sv.len)
		return false;

	i32	sign = 1;

	if (sv.buf[i] == '-' || sv.buf[i] == '+') {
		if (sv.buf[i] == '-')
			sign = -1;
		++i;
	}

	// Only sign character without digits
	if (i >= sv.len || sv.buf[i] < '0' || sv.buf[i] > '9')
		return false;

	i32	num = 0;

	while (i < sv.len && (sv.buf[i] >= '0' && sv.buf[i] <= '9')) {
		i32	digit = sv.buf[i] - '0';

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


bool sToI32(const String *str, i32 *out)
{
	return svToI32(sView(str), out);
}


size_t svNumberStartIdx(StringView sv)
{
	size_t	idx = 0;

	while (idx < sv.len && (sv.buf[idx] < '0' || sv.buf[idx] > '9'))
		++idx;

	if (idx >= sv.len)
		return SV_NOT_FOUND;

	if (idx > 0 && (sv.buf[idx - 1] == '-' || sv.buf[idx - 1] == '+'))
		return (idx - 1);

	return idx;
}


size_t svNumberEndIdx(StringView sv)
{
	size_t	idx = svNumberStartIdx(sv);

	if (idx == SV_NOT_FOUND)
		return SV_NOT_FOUND;

	if (sv.buf[idx] == '-' || sv.buf[idx] == '+')
		++idx;

	while (idx < sv.len && sv.buf[idx] >= '0' && sv.buf[idx] <= '9')
		++idx;

	return idx;
}


size_t sNumberStartIdx(const String *str)
{
	return svNumberStartIdx(sView(str));
}


size_t sNumberEndIdx(const String *str)
{
	return svNumberEndIdx(sView(str));
}
