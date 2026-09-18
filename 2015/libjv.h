#ifndef LIBJV_H
# define LIBJV_H

# include <stdlib.h>
# include <stdint.h>
# include <string.h>
# include <stdbool.h>


# define SV_NOT_FOUND ((size_t)-1)


typedef uint8_t		u8;
typedef uint16_t	u16;
typedef uint32_t	u32;
typedef uint64_t	u64;

typedef int8_t		i8;
typedef int16_t		i16;
typedef int32_t		i32;
typedef int64_t		i64;

typedef uintptr_t	uPtr;


typedef struct {
	size_t	len;	// unsigned long, so also 32 or 64, most likely 64
	size_t	cap;	// 0 if on stack/immutable?
	char	*buf;	// pointer size is 32 or 64 bits
} String;

typedef struct {
	size_t		len;
	const char	*buf;
} StringView;


ssize_t	print(const String *str);
ssize_t	printLine(const String *str);

String	*sEmpty(void);
String	*sCopy(const String *str);
String	*sCopyTo(const String *src, String *dest);
String	*sCopy(const String *str);
String	*sCopyCStringTo(const char *src, String *dest);
String	*sJoin(const String *s1, const String *s2);
String	*sAppend(String *s1, const String *s2);
String	*sReserve(String *str, size_t size);
String	*sResize(String *str, size_t cap);
void	sFreeBuf(String *str_ptr);
void	sDestroy(String **ptr);

StringView	sView(const String *str);
StringView	svSlice(const StringView sv, i64 start, i64 end);
StringView	svSliceFrom(const StringView sv, i64 start);
StringView	sSlice(const String *str, i64 start, i64 end);

i32	sCompare(const String *s1, const String *s2);
i32	sCompareN(const String *s1, const String *s2, size_t n);

i32	svCompare(StringView sv1, StringView sv2);
i32	svCompareN(StringView sv1, StringView sv2, size_t n);

String	*i64ToString(i64 num);
String	*i64IntoString(i64 num, String *str);

size_t	u64DecimalDigits(u64 num);
size_t	i64DecimalDigits(i64 num);

bool	svToI32(StringView sv, i32 *out);
bool	sToI32(const String *str, i32 *out);

size_t	svNumberStartIdx(StringView sv);
size_t	svNumberEndIdx(StringView sv);
size_t	sNumberStartIdx(const String *str);
size_t	sNumberEndIdx(const String *str);


# define strLen(str) (str).len
# define stackString(str) (String){.buf = (str), .len = strlen((str)), .cap = 0}

#endif
