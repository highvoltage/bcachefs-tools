#ifndef _LIB_KSTRTOX_H
#define _LIB_KSTRTOX_H

const char *_parse_integer_fixup_radix(const char *s, unsigned int *base);
unsigned int _parse_integer(const char *s, unsigned int base, unsigned long long *res);

#endif
