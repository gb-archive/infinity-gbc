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

int main(int argc, char *argv[])
{
        unsigned char c;
        FILE *in, *out;
        int n;

        if(argc < 4)
                return 1;
        in = fopen(argv[1], "rb");
        out = fopen(argv[2], "wb");
        if(!in || !out)
                return 1;

        fprintf(out, "unsigned char %s[] = {\n", argv[3]);
        n = 0;

        while(1) {
                c = fgetc(in);
                if(feof(in))
                        break;
                if(n == 0)
                        fprintf(out, "  ");

                fprintf(out, "0x%02x,", c);
                ++n;

                if(n == 16) {
                        fprintf(out, "\n");
                        n = 0;
                }
        }
        if(n != 0)
                fprintf(out, "\n");
        fprintf(out, "};\n");

        fclose(in);
        fclose(out);
}
