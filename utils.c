#include "ft_malcolm.h"

void *ft_memset(void *s, int c, size_t n)
{
	unsigned char *ptr;
	size_t         i;

	ptr = (unsigned char *)s;
	i = 0;
	while (i < n)
		ptr[i++] = (unsigned char)c;
	return (s);
}

void *ft_memcpy(void *dst, const void *src, size_t n)
{
	unsigned char       *d;
	const unsigned char *s;
	size_t               i;

	d = (unsigned char *)dst;
	s = (const unsigned char *)src;
	i = 0;
	while (i < n)
	{
		d[i] = s[i];
		i++;
	}
	return (dst);
}

int ft_memcmp(const void *s1, const void *s2, size_t n)
{
	const unsigned char *a;
	const unsigned char *b;
	size_t               i;

	a = (const unsigned char *)s1;
	b = (const unsigned char *)s2;
	i = 0;
	while (i < n)
	{
		if (a[i] != b[i])
			return (a[i] - b[i]);
		i++;
	}
	return (0);
}

int ft_strcmp(const char *s1, const char *s2)
{
	size_t i;

	i = 0;
	while (s1[i] && s1[i] == s2[i])
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

size_t ft_strlen(const char *s)
{
	size_t i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

unsigned long ft_strtoul(const char *ptr, char **endptr, int base)
{
	unsigned long result;
	int           digit;

	result = 0;
	while (*ptr)
	{
		if (*ptr >= '0' && *ptr <= '9')
			digit = *ptr - '0';
		else if (*ptr >= 'a' && *ptr <= 'f')
			digit = *ptr - 'a' + 10;
		else if (*ptr >= 'A' && *ptr <= 'F')
			digit = *ptr - 'A' + 10;
		else
			break ;
		if (digit >= base)
			break ;
		result = result * (unsigned long)base + (unsigned long)digit;
		ptr++;
	}
	if (endptr)
		*endptr = (char *)ptr;
	return (result);
}
