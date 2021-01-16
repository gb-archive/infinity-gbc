/*
 * Copyright (C) 1999-2001 Affinix Software, LLC
 *
 * This file is part of Infinity.
 *
 * This file may be used under the terms of the Creative Commons Attribution-
 * NonCommercial-ShareAlike 4.0 International License as published by Creative
 * Commons.
 *
 * Alternatively, this file may be used under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either version
 * 3 of the License, or (at your option) any later version.
 *
 * In addition, as a special exception, Affinix Software gives you certain
 * additional rights. These rights are described in the LICENSE file in this
 * package.
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<memory.h>

unsigned char *buf;
char title[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x80};

#define OP_JP  0xC3;

int find_deadbeef(unsigned char **addr, int size)
{
   int n;
   unsigned char *buf;
   char str[4] = { 0xde, 0xad, 0xbe, 0xef };

   buf = *addr;
   for(n = 0; n < size - 5; ++n) {
      if(!memcmp(buf + n, str, 4)) {
         *addr = buf + n + 4;
         return 1;
      }
   }
   return 0;
}

int main(int argc, char *argv[])
{
   FILE *f;
   unsigned int total;
   int total2;
   unsigned char high, low;
   int size, n, x, dest, len;
   unsigned char *addr;

   if(argc < 3)
      exit(0);

   f = fopen(argv[1], "r+b");
   if(!f)
      exit(0);
   fseek(f, 0l, SEEK_END);
   size = ftell(f);
   buf = malloc(size);
   if(!buf)
      exit(0);
   rewind(f);
   fread(buf, size, 1, f);
   rewind(f);

   addr = buf;
   while(find_deadbeef(&addr, size - (addr - buf))) {
      x = addr - buf;
      dest = (buf[x+1] << 8) + buf[x];
      len = buf[x+2];

      printf("patch offset:[%06X] length:[%d] bytes\n", x, len);

      memcpy(buf + dest, buf + x + 3, len);
   }

   // 00 - copy to location
   // uword dest addr
   // ubyte size

/* n = find_str(buf, (unsigned char[]) { 0xde, 0xad, 0xbe, 0xef, 0x01, 0x00 } );
   if(n) {
      buf[0x48] = OP_JP;
      buf[0x49] = (n&0xff);
      buf[0x4a] = (n&0xffff) >> 8;
   }

   // locate vblank
   n = find_str(buf, (unsigned char[]) { 0xde, 0xad, 0xbe, 0xef, 0x02, 0x00 } );
   if(n) {
      buf[0x40] = OP_JP;
      buf[0x41] = (n&0xff);
      buf[0x42] = (n&0xffff) >> 8;
   }

   // locate timer interrupt
   n = find_str(buf, (unsigned char[]) { 0xde, 0xad, 0xbe, 0xef, 0x03, 0x00 } );
   if(n) {
      buf[0x50] = OP_JP;
      buf[0x51] = (n&0xff);
      buf[0x52] = (n&0xffff) >> 8;
   }*/

   // title
   n = strlen(argv[2]);
   strncpy(title, argv[2], n);
   memcpy(buf + 0x134, title, 16);

   // stack
//   buf[0x155] = 0xD0;
// buf[0x154] = 0xC0;

   // complement
   total2 = -25;
   for(n = 0x134; n <= 0x14c; ++n)
      total2 -= buf[n];
   buf[0x14d] = total2 & 0xff;

   // checksum
   total = 0;
   for(n = 0; n < 0x14E; ++n)
      total += buf[n];
   for(n = 0x150; n < size; ++n)
      total += buf[n];
   total &= 0xffff;
   high = (total >> 8) & 0xff;
   low  =  total       & 0xff;
   buf[0x14E] = high;
   buf[0x14F] = low;

   fwrite(buf, size, 1, f);
   fclose(f);

   printf("%s patched\n", argv[1]);
}
