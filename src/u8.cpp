#include "common.hpp"
#include "u8.hpp"
#include "stringutil.hpp"

/* returns a unicode character or EOF
   doesn't handle more than 4-byte sequences
   Valid utf-8 sequences look like this :
   0xxxxxxx
   110xxxxx 10xxxxxx
   1110xxxx 10xxxxxx 10xxxxxx
   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
   1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 */
int u8_getc(u8_getc_fn get, u8_ungetc_fn unget, void *ud) {
	int c[4];
	c[0] = get(ud);
	if (c[0] < 0x80) return c[0];
	if (c[0] < 0xC2) goto error;
	c[1] = get(ud);
	if (c[1] < 0) return c[1];
	if ((c[1] & 0xc0) != 0x80) goto error1;
	if (c[0] < 0xe0) {
		return (c[0] << 6) + c[1] - 0x3080;
	} else if (c[0] < 0xf0) {
		if (c[0] == 0xe0 && c[1] < 0xa0) goto error1; /* overlong */
		c[2] = get(ud);
		if (c[2] < 0) return c[2];
		if ((c[2] & 0xc0) != 0x80) goto error2;
		return (c[0] << 12) + (c[1] << 6) + c[2] - 0xe2080;
	} else if (c[0] < 0xf5) {
		if (c[0] == 0xf0 && c[1] < 0x90) goto error1; /* overlong */
		if (c[0] == 0xf4 && c[1] >= 0x90) goto error1; /* > U+10FFFF */
		c[2] = get(ud);
		if (c[2] < 0) return c[2];
		if ((c[2] & 0xc0) != 0x80) goto error2;
		c[3] = get(ud);
		if (c[3] < 0) return c[3];
		if ((c[2] & 0xc0) != 0x80) goto error3;
		return (c[0] << 18) + (c[1] << 12) + (c[2] << 6) + c[3] - 0x3c82080;
	}
	/* > U+10FFFF */
	goto error;
error3: unget(c[3], ud);
error2: unget(c[2], ud);
error1: unget(c[1], ud);
error: return '?';
}

int u8_append(char* buf, int ch, int len)
{
	char tmp[5] = {0};
	if (ch < 0x80) {
		tmp[0] = (uint8_t)(ch);
	} else if (ch < 0x800) {
		tmp[0] = (uint8_t)((ch >> 6) | 0xc0);
		tmp[1] = (uint8_t)((ch & 0x3f) | 0x80);
	} else if (ch < 0x10000) {
		tmp[0] = (uint8_t)((ch >> 12) | 0xe0);
		tmp[1] = (uint8_t)(((ch >> 6) & 0x3f) | 0x80);
		tmp[2] = (uint8_t)((ch & 0x3f) | 0x80);
	} else {
		tmp[0] = (uint8_t)((ch >> 18) | 0xf0);
		tmp[1] = (uint8_t)(((ch >> 12) & 0x3f) | 0x80);
		tmp[2] = (uint8_t)(((ch >> 6) & 0x3f) | 0x80);
		tmp[3] = (uint8_t)((ch & 0x3f) | 0x80);
	}
	return strmcat(buf, tmp, len);
}

int u8_append(string& buf, int ch)
{
	char tmp[5] = {0};
	if (ch < 0x80) {
		tmp[0] = (uint8_t)(ch);
	} else if (ch < 0x800) {
		tmp[0] = (uint8_t)((ch >> 6) | 0xc0);
		tmp[1] = (uint8_t)((ch & 0x3f) | 0x80);
	} else if (ch < 0x10000) {
		tmp[0] = (uint8_t)((ch >> 12) | 0xe0);
		tmp[1] = (uint8_t)(((ch >> 6) & 0x3f) | 0x80);
		tmp[2] = (uint8_t)((ch & 0x3f) | 0x80);
	} else {
		tmp[0] = (uint8_t)((ch >> 18) | 0xf0);
		tmp[1] = (uint8_t)(((ch >> 12) & 0x3f) | 0x80);
		tmp[2] = (uint8_t)(((ch >> 6) & 0x3f) | 0x80);
		tmp[3] = (uint8_t)((ch & 0x3f) | 0x80);
	}
	buf += tmp;
	return buf.length();
}
