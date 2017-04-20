#include "oc_dump.h"

void stdio_dump(const uint8_t *buf, size_t len)
{
  int i;
  char *c;
  
  c = "0123456789abcdef";
  putchar('<');
  for(i = 0; i <len; i++) {
    if (i%4 == 0 && i != 0) {
      putchar(' ');
    }
    putchar(c[(buf[i]&0xf0)>>4]);
    putchar(c[buf[i]&0x0f]);
  }
  putchar('>');
}
