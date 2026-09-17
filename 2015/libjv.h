#ifndef LIBJV_H
# define LIBJV_H


# include <stdio.h>
# include <stddef.h>
# include <string.h>
# include <unistd.h>
# include <stdlib.h>
# include <stdint.h>
# include <limits.h>


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
	size_t	len;
	char	*buf;
} StringView;


ssize_t	print(const String *str);
ssize_t	printLine(const String *str);

String	*emptyStringPtr(void);
String	*sCopy(const String *str);
String	*sCopyTo(const String *src, String *dest);
String	*sCopyCStringTo(const char *src, String *dest);
String	*sJoin(const String *s1, const String *s2);
String	*sConcatTo(String *s1, const String *s2);
String	*sReserve(String *str, size_t size);
void	sFreeBuf(String *str_ptr);
void	sFreePtr(String **ptr);

StringView	sView(const String *str, i32 start, i32 end);

i32	sCompare(const String *s1, const String *s2);
i32	sCompareN(const String *s1, const String *s2, size_t n);

String	*i64ToString(i64 num);
String	*i64IntoString(i64 num, String *str);

size_t	u64DecimalDigits(u64 num);
size_t	i64DecimalDigits(i64 num);


# define strLen(str) (str).len
# define stackString(str) (String){.buf = (str), .len = strlen((str)), .cap = 0}

#endif
