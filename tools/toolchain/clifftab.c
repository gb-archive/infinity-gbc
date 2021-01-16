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

unsigned char buf[16384];

main()
{
	FILE *f;
	int n;
	int top, middle, bottom;
	int atline;
	int len;
	int ytable[256], tab[256];
	int deep, fake_len;

	memset(buf, 0, 16384);

	for(atline = 0; atline < 240; ++atline) {
		len = atline / 2;
		if(len > 48)
			len = 48;

		middle = 144 - atline + len;

		if(atline >= 144) {
			top = 0;
			deep = atline - 144;
			fake_len = len - deep;
		}
		else
			top = 144 - atline;

		if(atline >= 96)
			bottom = 240 - atline;
		else
			bottom = 144;

		for(n = 0; n < len; ++n) {
			ytable[n] = n * 48 / len;
		}

		printf("top=%d,middle=%d,bottom=%d,len=%d", top, middle, bottom, len);
		if(top == 0) {
			printf(" (deep=%d,FL=%d)", deep, fake_len);
		}
		printf("\n");
		bioskey(0);

		// if top is out of bounds
		if(top == 0) {
			for(n = 0; n < fake_len; ++n)
				tab[n] = ytable[n + deep];
			len = fake_len;
		}
		else {
			for(n = 0; n < len; ++n)
				tab[n] = ytable[n];
		}

		buf[atline * 64 + 0] = top;
		buf[atline * 64 + 1] = middle;
		buf[atline * 64 + 2] = bottom;
		buf[atline * 64 + 3] = len;
		for(n = 0; n < len; ++n)
			buf[atline * 64 + 4 + n] = tab[n];
	}


	f = fopen("clifftab.pag", "wb");
	fwrite(buf, 16384, 1, f);
	fclose(f);
}
