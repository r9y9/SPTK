/* ----------------------------------------------------------------- */
/*             The Speech Signal Processing Toolkit (SPTK)           */
/*             developed by SPTK Working Group                       */
/*             http://sp-tk.sourceforge.net/                         */
/* ----------------------------------------------------------------- */
/*                                                                   */
/*  Copyright (c) 1984-2007  Tokyo Institute of Technology           */
/*                           Interdisciplinary Graduate School of    */
/*                           Science and Engineering                 */
/*                                                                   */
/*                1996-2013  Nagoya Institute of Technology          */
/*                           Department of Computer Science          */
/*                                                                   */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/* - Redistributions of source code must retain the above copyright  */
/*   notice, this list of conditions and the following disclaimer.   */
/* - Redistributions in binary form must reproduce the above         */
/*   copyright notice, this list of conditions and the following     */
/*   disclaimer in the documentation and/or other materials provided */
/*   with the distribution.                                          */
/* - Neither the name of the SPTK working group nor the names of its */
/*   contributors may be used to endorse or promote products derived */
/*   from this software without specific prior written permission.   */
/*                                                                   */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND            */
/* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,       */
/* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF          */
/* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE          */
/* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS */
/* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,          */
/* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   */
/* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     */
/* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON */
/* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,   */
/* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY    */
/* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE           */
/* POSSIBILITY OF SUCH DAMAGE.                                       */
/* ----------------------------------------------------------------- */

/************************************************************************
*                                                                       *
*    convert raw file format to wav file format                         *
*                                                                       *
*                                       2009.9 A.Saito modified         *
*       usage:                                                          *
*               rawtowav [ fs(Hz) ] [ infile ] [ outfile ]              *
*       infile:                                                         *
*               raw file format                                         *
*       outfile:                                                        *
*               wav file format                                         *
*                                                                       *
************************************************************************/
#include<stdio.h>
#include<stdlib.h>

#if defined(WIN32)
#  include "SPTK.h"
#else
#  include <SPTK.h>
#endif

void write_file(long fs, char BIT, char *rawfile, char *wavfile)
{
   FILE *fpi, *fpo;
   char RIFF[] = "RIFF";
   char WAVE[] = "WAVE";
   char fmt_chunk[] = "fmt ";
   char data_chunk[] = "data";
   int file_size, rawfile_size;
   int chunk_size = BIT;
   int data_speed;
   short formatID = 1;
   short channel = 1;           /* mono:1¡¤stereo:2 */
   short block_size;            /* 16bit, mono => 16bit*1=2byte */
   short bit;
   int c, buf[2] = { 0, 0 };

   fpi = getfp(rawfile, "rb");
   fpo = getfp(wavfile, "wb");

   fseek(fpi, 0, SEEK_END);
   rawfile_size = ftell(fpi);
   file_size = rawfile_size + 36;
   fseek(fpi, 0, SEEK_SET);

   /* RIFF header */
   fwritex(RIFF, sizeof(char), 4, fpo);
   /* file size */
   buf[0] = file_size;
   fwrite_little_endian(buf, 4, 1, fpo);
   /* WAVE header */
   fwritex(WAVE, sizeof(char), 4, fpo);
   /* fmt chunk */
   fwritex(fmt_chunk, sizeof(char), 4, fpo);
   /* chunk size */
   buf[0] = chunk_size;
   fwrite_little_endian(buf, 4, 1, fpo);
   /* formatID */
   fwritex(&formatID, sizeof(short), 1, fpo);
   /* channel (mono:1¡¤stereo:2) */
   fwritex(&channel, sizeof(short), 1, fpo);
   /* sampling frequency */
   buf[0] = fs;
   fwrite_little_endian(buf, 4, 1, fpo);
   /* data speed */
   data_speed = fs * BIT / 8 * formatID;
   buf[0] = data_speed;
   fwrite_little_endian(buf, 4, 1, fpo);
   /* block size */
   block_size = BIT / 8 * formatID;
   fwritex(&block_size, sizeof(short), 1, fpo);
   /* bit number */
   bit = BIT;
   fwritex(&bit, sizeof(short), 1, fpo);
   /* data chunk */
   fwritex(data_chunk, sizeof(char), 4, fpo);
   /* file size of data */
   buf[0] = rawfile_size;
   fwrite_little_endian(buf, 4, 1, fpo);
   while ((c = fgetc(fpi)) != EOF)
      fputc(c, fpo);

   fclose(fpi);
   fclose(fpo);
}


int main(int argc, char **argv)
{

   if (argc != 5) {
      printf("error : failed to convert raw to wav\n\n");
      printf("rawtowav : convert raw to wav\n");
      printf("usage:\n");
      printf("        rawtowav [ fs(Hz) ] [ infile ] [ outfile ]\n");
      exit(0);
   }

   write_file(atol(argv[1]), (char) atoi(argv[2]), argv[3], argv[4]);

   return (0);
}
