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


ssize_t	print(const String *str);
ssize_t	printLine(const String *str);

String	*emptyString(void);
String	*stringCopy(const String *str);
String	*stringJoin(const String *s1, const String *s2);
String	*stringConcatTo(String *s1, const String *s2);
String	*stringReserve(String *str, size_t size);
void	stringFree(String **str_ptr);

String	*i64ToString(i64 num);
String	*i64IntoString(i64 num, String *str);

size_t	u64DecimalDigits(u64 num);
size_t	i64DecimalDigits(i64 num);


# define strLen(str) (str).len
# define stackString(str) (String){.buf = (str), .len = strlen(str), .cap = 0}

#endif
