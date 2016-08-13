/*
 * Copyright (C) 1999-2001 Affinix Software, LLC
 *
 * This file is part of Infinity.
 *
 * This file may be used under the terms of the Creative Commons Attribution-
 * NonCommercial-ShareAlike 4.0 International License as published by Creative
 * Commons.
 *
 * Alteratively, this file may be used under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either version
 * 3 of the License, or (at your option) any later version.
 *
 * In addition, as a special exception, Affinix Software gives you certain
 * additional rights. These rights are described in the LICENSE file in this
 * package.
 */

#include <windows.h>

void Directory(char path[])
{
    char subpath[_MAX_PATH];
    char temp[_MAX_PATH];
    WIN32_FIND_DATA lp;

    strcpy(temp,path);
    HANDLE h=FindFirstFile(temp,&lp);

    temp[strlen(temp)-1]='\0';//*を消している
    do
    {      
//        if((lp.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
//            && strcmp(lp.cFileName,"..")!=0 && strcmp(lp.cFileName,".")!=0)
//        {            
//           //subpathでディレクトリのフルパスが分かる
//            wsprintf(subpath,"%s%s\\*",temp,lp.cFileName);
//            Directory(subpath);
//        }
//        if((lp.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)!=FILE_ATTRIBUTE_DIRECTORY)
//        {
//           // lp.cFilenameでファイル名が分かる
//           // wsprintf(出力文字列,"%s%s",temp,lp.cFileName);とすれば
//           // ファイルのフルパスが分かる
//
//           //こちらはファイルサイズ
//           //size+=lp.nFileSizeHigh*MAXDWORD+lp.nFileSizeLow ;
//           
//        }        
    } while(FindNextFile(h,&lp));
 
        FindClose(h);
}

char **fname;
FILE *fp;
//DIR *dp;
struct dirent *d;
byte datastruct[8];
int num_tiles, num_metas;
int width, height;
byte map_type;

char **get_file_list(int *num, char *ext)
{
   char **flist;
   flist = (char **) malloc(4);
//   flist[0] = strdup("foobar");
//   *num = 1;
//   return flist;


//   char *str;
   char str[1024];
   int n;

//   flist = NULL;
//   dp = opendir(".");

    char temp[_MAX_PATH];
    WIN32_FIND_DATA lp;
    wsprintfA(temp,"*%s", ext);
    HANDLE h=FindFirstFile(temp,&lp);

	n = 0;

    do
    {
//        if((lp.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
//            && strcmp(lp.cFileName,"..")!=0 && strcmp(lp.cFileName,".")!=0)
//        {            
//           //subpathでディレクトリのフルパスが分かる
//            wsprintf(subpath,"%s%s\\*",temp,lp.cFileName);
//            Directory(subpath);
//        }
        if((lp.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)!=FILE_ATTRIBUTE_DIRECTORY)
        {
			wsprintfA(str, "%s", lp.cFileName);
			flist = (char **) realloc((void *)flist, (n+1)*sizeof(char *));
			flist[n++] = strdup(str);
//           // lp.cFilenameでファイル名が分かる
//           // wsprintf(出力文字列,"%s%s",temp,lp.cFileName);とすれば
//           // ファイルのフルパスが分かる
//
//           //こちらはファイルサイズ
//           //size+=lp.nFileSizeHigh*MAXDWORD+lp.nFileSizeLow ;
//           
        }
    } while(FindNextFile(h, &lp));
 
	FindClose(h);

//   for (n = 0;;)
//   {
////      d = readdir(dp);
//      if (!d)
//         break;
//
////      str = d->d_name;
//	  str = "abc";
//
//      if (strstr(str, ext))
//      {
//         flist = (char **) realloc((void *)flist, (n+1)*sizeof(char *));
//         flist[n++] = strdup(str);
//      }
//   }
//
////   closedir(dp);

   *num = n;

   return flist;
}

void clear_file_list(int num, char **flist)
{
   int i;
   for (i=0; i<num; i++)
      free(flist[i]);
   free(flist);
}

void new_tileset()
{
   int i;

   num_tiles = num_metas = 1;
   memset(palettes, 0, 64);
   memset(gbapalettes, 0, 64);
   memset(palettetable, 0, 256);
   memset(tilebuf, 0, 4096);
   memset(tile16, 0, 1024);
   memset(attr, 0, 1024);

   for(i=0;i<256;i++)
      conv_tile(i);

   for (i=0;i<8;i++)
   {
      set_comp(palettes,i * 4,pal_r,31);
      set_comp(palettes,i * 4,pal_g,31);
      set_comp(palettes,i * 4,pal_b,31);
      set_comp(palettes,i*4+1,pal_r,14);
      set_comp(palettes,i*4+1,pal_g,14);
      set_comp(palettes,i*4+1,pal_b,14);
      set_comp(palettes,i*4+2,pal_r,10);
      set_comp(palettes,i*4+2,pal_g,10);
      set_comp(palettes,i*4+2,pal_b,10);
      set_comp(palettes,i*4+3,pal_r, 0);
      set_comp(palettes,i*4+3,pal_g, 0);
      set_comp(palettes,i*4+3,pal_b, 0);
   }
}

int open_tileset(char *file)
{
   int i;

   new_tileset();

   if (fp = fopen(file, "rb"))
   {
      fread(id, 1, 3, fp);

      if (memcmp(id, idchk_tile, 3))
         return -1;

      fread(&num_tiles, 4, 1, fp);
      fread(&num_metas, 4, 1, fp);
   
      fread(palettes, 1, 64, fp);
      fread(gbapalettes, 1, 64, fp);
      fread(an_tiles, 1, 8, fp);
      fread(an_index, 1, 8, fp);
      fread(palettetable, 1, num_tiles, fp);

      if (num_metas)
         for (i = 0; i < num_metas; i++)
         {   
            fread(datastruct, 8, 1, fp);
   
            tile16[i * 4] = datastruct[0];
            tile16[i*4+1] = datastruct[1];
            tile16[i*4+2] = datastruct[2];
            tile16[i*4+3] = datastruct[3];
            attr  [i * 4] = datastruct[4];
            attr  [i*4+1] = datastruct[5];
            attr  [i*4+2] = datastruct[6];
            attr  [i*4+3] = datastruct[7];
         }

      fread(tilebuf, 16, num_tiles, fp);

      for(i=0;i<256;i++)
         conv_tile(i);

      fclose(fp);
      return 0;
   }
   else
      return 1;
}

int save_tileset(char *file)
{
   int i,j;

   num_metas=1;   
   for (j=0;j<256;j++)
   {
      for (i=0;i<4;i++)
      if (tile16[j*4+i])
         num_metas=j+1;
   }

   num_tiles=1;   
   for (j=0;j<256;j++)
   {
      for (i=0;i<16;i++)
      if (tilebuf[j*16+i])
         num_tiles=j+1;
   }

   if (!(fp = fopen(file, "wb")))
      return 1;

   fwrite(idchk_tile, 1, 3, fp);
   fwrite(&num_tiles, 4, 1, fp);
   fwrite(&num_metas, 4, 1, fp);

   fwrite(palettes, 1, 64, fp);
   fwrite(gbapalettes, 1, 64, fp);
   fwrite(an_tiles, 1, 8, fp);
   fwrite(an_index, 1, 8, fp);
   fwrite(palettetable, 1, num_tiles, fp);

   for (i = 0; i < num_metas; i++)
   {
      datastruct[0]=(tile16[i * 4]);
      datastruct[1]=(tile16[i*4+1]);
      datastruct[2]=(tile16[i*4+2]);
      datastruct[3]=(tile16[i*4+3]);

      datastruct[4]=attr[i * 4];
      datastruct[5]=attr[i*4+1];
      datastruct[6]=attr[i*4+2];
      datastruct[7]=attr[i*4+3];

      fwrite(datastruct, 8, 1, fp);
   }
   fwrite(tilebuf, 16, num_tiles, fp);

   fclose(fp);

   return 0;
}

void new_map()
{
   int i;

   width=height=32;
   for (i=0;i<16384;i++)
      map[i]=0;
}

int open_map(char *file)
{
   int i;

   new_map();

   if (fp = fopen(file, "rb"))
   {
      fread(id, 1, 3, fp);

      if (memcmp(id, idchk_map, 3))
         return -1;

      fread(&width, 4, 1, fp);
      fread(&height, 4, 1, fp);
   
      fread(&map_type, 1, 1, fp);

      for (i=0;i<height;i++)
         fread(&map[i*128], width, 1, fp);

      if((map_type&3)==3)
         for(i=0;i<height;i++)
            fread(&en[i*128],width,1,fp);

      fclose(fp);
      return 0;
   }
   else
      return 1;
}

void fix_mapsize()
{
   int i,j;

   width=1;
   for(i=0;i<128;i++)
      for(j=0;j<128;j++)
         if(map[j*128+i])
            width=i+1;

   height=1;
   for(j=0;j<128;j++)
      for(i=0;i<128;i++)
         if(map[j*128+i])
            height=j+1;

   map_type&=0xFC;
   if(width>=64&&height>=64)
      map_type|=0x03,width=128,height=128;
   if(width>=64&&height<32)
      map_type|=0x02;
   if(width<32&&height>=64)
      map_type|=0x01;
   if(width<64&&height<64)
      map_type|=0x00;
}

int save_map(char *file)
{
   int i;

   fix_mapsize();

   if(!(fp = fopen(file, "wb")))
      return 1;

   fwrite(idchk_map, 1, 3, fp);
   fwrite(&width, 4, 1, fp);
   fwrite(&height, 4, 1, fp);
   fwrite(&map_type, 1, 1, fp);

   for(i=0;i<height;i++)
      fwrite(&map[i*128],width,1,fp);

   if((map_type&3)==3)
      for(i=0;i<height;i++)
         fwrite(&en[i*128],width,1,fp);

   fclose(fp);

   return 0;
}

/*struct pcxheader {
   byte man, ver, enc, bit;
   short xls, yls, xms, yms;
   short hre, vre;
   byte col[48];
   byte res, pla;
   short byt, pal;
   byte fil[58];
};*/

/*
void loadpcx(pixel* screen, int x, int y, byte* filename)
{
   FILE *fp;

   int i, j;
   int sizex, sizey;
   byte clr, lps;
   byte tmptext[100];
   byte* palette;
   short* newpalette;
   pcxheader pcxhead;

   palette = (byte*)malloc(768);
   newpalette = (short*)malloc(512);

   if ( NULL == (fp = fopen(filename, "rb")) )
      fatalerror("Error opening file.");

   fseek(fp, -768, SEEK_END);
   fread(palette, 768, 1, fp);

   fseek(fp, 0, SEEK_SET);
   fread(&pcxhead, sizeof(pcxhead), 1, fp);

   for(i = 0; i < 256; i++)
      newpalette[i] = clr16(palette[i*3]/8,palette[i*3+1]/8,palette[i*3+2]/8);

   sizex = pcxhead.xms; sizey = pcxhead.yms;

   fseek(fp, 128, SEEK_SET);
   
   i=0; j=0;
   
   while (j < sizey)
   {
      fread(&clr, 1, 1, fp);
   
      if (clr > 192)
      {
         lps = clr - 192;
         fread(&clr, 1, 1, fp);
      }
      else
         lps = 1;
   
      do
      {
         if ((x >= 0 && x < 640) && (y >= 0 && y < 480))
            putpixel(screen, x, y, newpalette[clr]);
         x++; i++;
         if (i > sizex)
         {
            i = 0; x -= (sizex + 1);
            y++; j++;
         }
         lps--;
      }
      while (lps != 0);
   }
   
   fclose(fp);
}
*/

byte pcxheader[]=
   {0x0A,0x05,0x01,0x08,0x00,0x00,0x00,0x00,
    0x3F,0x01,0xC7,0x00,0x40,0x01,0xC8,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x01,0x40,0x01,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

byte *pcxbuf;

void copy(int xs,int ys,
          byte *buf,int xbs,int ybs,int x,int y,
          byte *gfx,int xbss,int ybss,int xss,int yss)
{
   int i,j;

   for(j=0;j<ys;j++)
      for(i=0;i<xs;i++)
         buf[(y+j)*xbs+(x+i)]=gfx[(yss+j)*xbss+(xss+i)];
}

int save_mappcx()
{
   int r,g,b;
   int x,y;

   int xs,ys;

   int i,j,num;
   char buf[128], buf2[128];
   byte palbuf[768];

   fname=get_file_list(&i, ".PCX");

   for(num=0;num<10000;num++)
   {
      sprintf(buf,"MAP%04d.PCX",num);
      for(j=0;j<i;j++)
         if(!strcmp(buf,fname[j]))
            break;
      if(j>=i)
         break;
   }
   if(num<10000)
   {
      fix_mapsize();

      xs=width*17+17;
      ys=height*17+9;

      pcxbuf=new byte[xs*ys];
      for(j=0;j<xs*ys;j++)
         pcxbuf[j]=0;

      fp=fopen(buf,"wb");

      for (y=0;y<height;y++)
         for (x=0;x<width;x++)
         {
            draw_tile16(0,0,map[y*128+x],0x7F);
            copy(16,16,(byte*)pcxbuf,xs,ys,x*17+17,y*17+9,(byte*)vscreen,640,480,0,0);
         }

      for (x=0;x<width;x++)
      {
         sprintf(buf2,"%02X",x);
         glib.drawbar(0,0,15,7,0);
         draw_string(0,0,buf2,15);
         copy(16,8,(byte*)pcxbuf,xs,ys,x*17+17,0,(byte*)vscreen,640,480,0,0);
      }

      for (y=0;y<height;y++)
      {
         sprintf(buf2,"%02X",y);
         glib.drawbar(0,0,15,7,0);
         draw_string(0,0,buf2,15);
         copy(16,8,(byte*)pcxbuf,xs,ys,0,y*17+17,(byte*)vscreen,640,480,0,0);
      }

      for (x=16;x<xs;x+=17)
         for (y=8;y<ys;y++)
            pcxbuf[y*xs+x]=16;
            
      for (x=16;x<xs;x++)
         for (y=8;y<ys;y+=17)
            pcxbuf[y*xs+x]=16;

      pcxheader[8]=((xs-1)&255);
      pcxheader[9]=((xs-1)/256)&255;

      pcxheader[10]=((ys-1)&255);
      pcxheader[11]=((ys-1)/256)&255;

      pcxheader[12]=(xs&255);
      pcxheader[13]=(xs/256)&255;

      pcxheader[14]=(ys&255);
      pcxheader[15]=(ys/256)&255;

      pcxheader[66]=(xs&255);
      pcxheader[67]=(xs/256)&255;

      fwrite(pcxheader, sizeof(pcxheader), 1, fp);

      for(y=0;y<ys;y++)
         for(x=0;x<xs;)
         {
            j=pcxbuf[y*xs+(x++)];

            for(num=1;num<63;num++,x++)
            {
               if(x>=xs)
                  break;
               if(j!=pcxbuf[y*xs+x])
                  break;
            }
   
            if((num==1&&j>=192)||(num>1))
               fputc(192+num,fp);
   
            fputc(j,fp);         
         }

      delete [] pcxbuf;

      fputc(12, fp);

//      outp(0x3c7,0);

      glib.vrwait();
      for(j=0;j<256;j++)
      {
//         r=inp(0x3c9);
//         g=inp(0x3c9);
//         b=inp(0x3c9);
		 r = (g_palette[j] >> 16) & 0xFF;
		 g = (g_palette[j] >> 8) & 0xFF;
		 b = (g_palette[j] >> 0) & 0xFF;

//         if(r+g+b<30)
//            r=g=b=0;

         if(r+g+b<120)
            r=g=b=0;

         palbuf[j*3  ]=r;//*4;
         palbuf[j*3+1]=g;//*4;
         palbuf[j*3+2]=b;//*4;
      }

      fwrite(palbuf,256,3,fp);

      fclose(fp);
   }

   clear_file_list(i, fname);

   return 0;
}
