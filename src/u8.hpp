#pragma once

typedef uint32_t rune_t;

typedef int (*u8_getc_fn)(void*);
typedef void (*u8_ungetc_fn)(int, void*);

rune_t u8_getc(u8_getc_fn get, u8_ungetc_fn unget, void *ud);
int u8_append(char* buf, rune_t ch, int len);
int u8_append(string& buf, rune_t ch);

extern rune_t invalid_rune;
