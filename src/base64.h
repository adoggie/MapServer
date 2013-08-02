
#ifndef _BASE64_H
#define _BASE64_H
#include <stdio.h>
#include <stdlib.h>
#define restrict 
#define BASE64_LENGTH(inlen) ((((inlen) + 2) / 3) * 4)

void	base64_encode (const char *restrict in, size_t inlen,char *restrict out, size_t outlen);       
bool	base64_decode (const char *restrict in, size_t inlen,char *restrict out, size_t *outlen);
bool isbase64 (char ch);

size_t	base64_encode_alloc (const char *in, size_t inlen, char **out);
bool	base64_decode_alloc (const char *in, size_t inlen, char **out,size_t *outlen);

#endif
