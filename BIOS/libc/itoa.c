#include "stdio.h"
#include "string.h"

void reverse(char s[]) {
	int c, i, j;
	for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

// slightly modified from: https://stackoverflow.com/a/12386915
int itoa(int value, char *dst, int base)
{

	char tmp[16];// be careful with the length of the buffer
	char *tp = tmp;
	int i;
	unsigned v;
	
	int sign = (base == 10 && value < 0);	
	if (sign)
		v = -value;
	else
		v = (unsigned)value;

	while (v || tp == tmp) {
		i = v % base;
		v /= base;  

		if (i < 10)
			*tp++ = i+'0';
		else
			*tp++ = i + 'a' - 10;
	}

	int len = tp - tmp;

	if (sign) {
		*dst++ = '-';
		len++;
	}

	while (tp > tmp)
		*dst++ = *--tp;
	
	*dst = '\0';

	return len;
}


