#include "libjv.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


String	*md5BitStringFromString(String const *src, String *dest)
{
	// 64 * 8 = 640 - 2 * 64 = 640 - 128 = 512
	const size_t	total_bytes = src->len + 1 + 8;
	const size_t	padded_len = ((total_bytes + 63) / 64) * 64;

	if (dest->cap < padded_len)
		sReserve(dest, padded_len * 2);

	memcpy(dest->buf, src->buf, src->len);
	dest->buf[src->len] = 0x80;

	u64	bit_len = src->len * 8;
	u8	*len_ptr = (u8 *)&dest->buf[padded_len - 8];

	for (int i = 0; i < 8; ++i)
		len_ptr[i] = (bit_len >> (i * 8)) & 0xFF;

	dest->len = dest->cap = padded_len;

	return dest;
}


u32	s[64] = {
	7,	12,	17,	22,	7,	12,	17,	22,	7,	12,	17,	22,	7,	12,	17,	22,
	5,	9,	14,	20,	5,	9,	14,	20,	5,	9,	14,	20,	5,	9,	14,	20,
	4,	11,	16,	23,	4,	11,	16,	23,	4,	11,	16,	23,	4,	11,	16,	23,
	6,	10,	15,	21,	6,	10,	15,	21,	6,	10,	15,	21,	6,	10,	15,	21
};

u32	k[64] = {
	0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
	0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
	0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
	0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
	0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
	0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
	0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
	0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
	0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
	0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
	0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
	0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
	0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
	0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
	0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
	0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};


u32	leftRotate(u32 x, u32 offset)
{
	return (x << offset) | (x >> (32 - offset));
}


void calculateHash(const String *str, String *dest, u32 *res)
{
	String			*bs = md5BitStringFromString(str, dest);
	const size_t	chunks = bs->len / 64;

	u32	a0 = 0x67452301;
	u32	b0 = 0xefcdab89;
	u32	c0 = 0x98badcfe;
	u32	d0 = 0x10325476;

	u32	a = 0;
	u32	b = 0;
	u32	c = 0;
	u32	d = 0;

	u32 f = 0;
	u32 g = 0;

	u32	m[16] = {0};

	for (size_t chunk_idx = 0; chunk_idx < chunks; ++chunk_idx) {
		memcpy(m, bs->buf + chunk_idx * 64, 16 * sizeof(*m));

		a = a0;
		b = b0;
		c = c0;
		d = d0;

		for (size_t i = 0; i < 16; ++i) {
			f = (b & c) | ((~b) & d);
			g = i;

			f = f + a + k[i] + m[g];
			a = d;
			d = c;
			c = b;
			b = b + leftRotate(f, s[i]);
		}
		for (size_t i = 16; i < 32; ++i) {
			f = (d & b) | ((~d) & c);
			g = (5 * i + 1) & 15;

			f = f + a + k[i] + m[g];
			a = d;
			d = c;
			c = b;
			b = b + leftRotate(f, s[i]);
		}
		for (size_t i = 32; i < 48; ++i) {
			f = b ^ c ^ d;
			g = (3 * i + 5) & 15;

			f = f + a + k[i] + m[g];
			a = d;
			d = c;
			c = b;
			b = b + leftRotate(f, s[i]);
		}
		for (size_t i = 48; i < 64; ++i) {
			f = c ^ (b | (~d));
			g = (7 * i) & 15;

			f = f + a + k[i] + m[g];
			a = d;
			d = c;
			c = b;
			b = b + leftRotate(f, s[i]);
		}
		a0 += a;
		b0 += b;
		c0 += c;
		d0 += d;
	}

	res[0] = a0;
	res[1] = b0;
	res[2] = c0;
	res[3] = d0;
}


int main(void)
{
	// Known solution abcdef609043
	String	input = stackString("iwrupvqb");

	String	candidate = {0};
	String	num_string = {0};
	String	md5BitString = {0};
	u32		result[4] = {0};
	u8		*out_bytes = NULL;
	char	hash[33] = {0};
	size_t	append_number = 1;

	sReserve(&candidate, 32);
	sAppend(&candidate, &input);
	sReserve(&num_string, 32);
	sReserve(&md5BitString, 128);

	size_t	loop_limit = 100000000;
	while (1) {
		if (append_number >= loop_limit) {
			fprintf(stderr, "ERROR: limit reached\n");
			exit(1);
		}
		i64IntoString(append_number, &num_string);
		sAppend(&candidate, &num_string);
		calculateHash(&candidate, &md5BitString, result);

		out_bytes = (u8*)result;
		if (out_bytes[0] == 0 && out_bytes[1] == 0 && (out_bytes[2] & 0xFF) == 0)
			break;

		candidate.len = input.len;
		++append_number;
	}
	sFreeBuf(&candidate);
	sFreeBuf(&num_string);
	sFreeBuf(&md5BitString);

	printf("%zu\n", append_number);

	return append_number;
}
