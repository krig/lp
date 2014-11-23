#pragma once

typedef int (*u8_getc_fn)(void*);
typedef void (*u8_ungetc_fn)(int, void*);

int u8_getc(u8_getc_fn get, u8_ungetc_fn unget, void *ud);
int u8_append(char* buf, int ch, int len);
int u8_append(string& buf, int ch);
