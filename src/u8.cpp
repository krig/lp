#include "common.hpp"
#include "u8.hpp"
#include "stringutil.hpp"

rune_t invalid_rune = (rune_t)-1;

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
rune_t u8_getc(u8_getc_fn get, u8_ungetc_fn unget, void *ud) {
	int c[6];
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
	} else if ((c[0] & 0xf8) == 0xf0) {
		if (c[0] == 0xf0 && c[1] < 0x90) goto error1; /* overlong */
		if (c[0] == 0xf4 && c[1] >= 0x90) goto error1; /* > U+10FFFF */
		c[2] = get(ud);
		if (c[2] < 0) return c[2];
		if ((c[2] & 0xc0) != 0x80) goto error2;
		c[3] = get(ud);
		if (c[3] < 0) return c[3];
		if ((c[2] & 0xc0) != 0x80) goto error3;
		return (c[0] << 18) + (c[1] << 12) + (c[2] << 6) + c[3] - 0x3c82080;
	} else if ((c[0] & 0xfc) == 0xf8) {
		if ((c[1] & 0xc0) != 0x80) goto error1;
		c[2] = get(ud);
		if (c[2] < 0) return c[2];
		if ((c[2] & 0xc0) != 0x80) goto error2;
		c[3] = get(ud);
		if (c[3] < 0) return c[3];
		if ((c[3] & 0xc0) != 0x80) goto error3;
		c[4] = get(ud);
		if (c[4] < 0) return c[4];
		if ((c[4] & 0xc0) != 0x80) goto error4;
		return ((c[0] & 0x7) << 24) + (c[1] << 18) + (c[2] << 12) + (c[3] << 6) + c[4] - 0x2082080;
	} else if ((c[0] & 0xfe) == 0xfc) {
		if ((c[1] & 0xc0) != 0x80) goto error1;
		c[2] = get(ud);
		if (c[2] < 0) return c[2];
		if ((c[2] & 0xc0) != 0x80) goto error2;
		c[3] = get(ud);
		if (c[3] < 0) return c[3];
		if ((c[3] & 0xc0) != 0x80) goto error3;
		c[4] = get(ud);
		if (c[4] < 0) return c[4];
		if ((c[4] & 0xc0) != 0x80) goto error4;
		c[5] = get(ud);
		if (c[5] < 0) return c[5];
		if ((c[5] & 0xc0) != 0x80) goto error5;
		return ((c[0] & 0x1) << 30) + ((c[1] & 0x3f) << 24) + ((c[2] & 0x3f) << 18) + ((c[3] & 0x3f) << 12) + ((c[4] & 0x3f) << 6) + (c[5] & 0x3f);
	} else {
		goto error;
	}
error5: unget(c[5], ud);
error4: unget(c[4], ud);
error3: unget(c[3], ud);
error2: unget(c[2], ud);
error1: unget(c[1], ud);
error: return '?';
}

namespace {
	inline void u8_convert(uint8_t* tmp, rune_t ch)
	{
		if (ch < 0x80) {
			tmp[0] = ch;
		} else if (ch < 0x800) {
			tmp[0] = 0xc0 | (ch >> 6);
			tmp[1] = 0x80 | (ch & 0x3f);
		} else if (ch < 0x10000) {
			tmp[0] = 0xe0 | (ch >> 12);
			tmp[1] = 0x80 | ((ch >> 6) & 0x3f);
			tmp[2] = 0x80 | (ch & 0x3f);
		} else if (ch < 0x200000) {
			tmp[0] = 0xf0 | (ch >> 18);
			tmp[1] = 0x80 | ((ch >> 12) & 0x3f);
			tmp[2] = 0x80 | ((ch >> 6) & 0x3f);
			tmp[3] = 0x80 | (ch & 0x3f);
		} else if (ch < 0x4000000) {
			tmp[0] = 0xf8 | (ch >> 24);
			tmp[1] = 0x80 | ((ch >> 18) & 0x3f);
			tmp[2] = 0x80 | ((ch >> 12) & 0x3f);
			tmp[3] = 0x80 | ((ch >> 6) & 0x3f);
			tmp[4] = 0x80 | ((ch >> 0) & 0x3f);
		} else if (ch < 0x80000000) {
			tmp[0] = 0xfc | (ch >> 30);
			tmp[1] = 0x80 | ((ch >> 24) & 0x3f);
			tmp[2] = 0x80 | ((ch >> 18) & 0x3f);
			tmp[3] = 0x80 | ((ch >> 12) & 0x3f);
			tmp[4] = 0x80 | ((ch >> 6) & 0x3f);
			tmp[5] = 0x80 | ((ch >> 0) & 0x3f);
		} else {
			// invalid rune, don't append
		}
	}
}

int u8_append(char* buf, rune_t ch, int len)
{
	uint8_t tmp[7] = {0};
	u8_convert(tmp, ch);
	return strmcat(buf, (char*)tmp, len);
}

int u8_append(string& buf, rune_t ch)
{
	uint8_t tmp[7] = {0};
	u8_convert(tmp, ch);
	buf += (char*)tmp;
	return buf.length();
}
