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
/*                1996-2014  Nagoya Institute of Technology          */
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

/*****************************************************************************
*                                                                            *
*    play 16-bit linear PCM data                                             *
*                                                                            *
*                                       1998.1  T.Kobayashi                  *
*                                       2000.3  M.Tamura                     *
*                                                                            *
*       usage:                                                               *
*               dawrite [ options ] infile1 infile2 ... > stdout             *
*       options:                                                             *
*               -s s  :  sampling frequency (8,11.025,22.05,44.1kHz) [11]    *
*               -g g  :  gain (.., -2, -1, 0, 1, 2, ..)              [0]     *
*               -a a  :  amplitude gain (0..100)                     [N/A]   *
*               -H H  :  header size in byte                         [0]     *
*               -v    :  display filename                            [FALSE] *
*               -w    :  byteswap                                    [FALSE] *
*               +x    :  data format                                 [s]     *
*                          s (short)   f (float)   d (double)                *
*       infile:                                                              *
*               data                                           [stdin]       *
*       notice:                                                              *
*               number of infile < 128                                       *
*                                                                            *
*****************************************************************************/

static char *rcs_id = "$Id: dawrite.c,v 1.33 2014/12/11 08:30:32 uratec Exp $";


/* Standard C Libraries */
#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_STRING_H
#  include <string.h>
#else
#  include <strings.h>
#  ifndef HAVE_STRRCHR
#     define strrchr rindex
#  endif
#endif

#ifndef WIN32
#  include <unistd.h>
#  include <SPTK.h>
#  include "da.h"
#else
#  include "SPTK.h"
#  include "da.h"
#  include "winplay.h"
#endif

/* Default Value */
#define SIZE     256*200
#define MAXFILES 128
#define INITGAIN 0

#define OUTPORT     's'
#define GAIN        (0+INITGAIN)
#define HEADERSIZE  0
#define VERBOSE     FA

char *BOOL_STR[] = { "FALSE", "TRUE" };

/* Command Name */
char *cmnd;

void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - play 16-bit linear PCM data\n\n", cmnd);
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] infile1 infile2 ... > stdout\n",
           cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -s s  : sampling frequency (%s kHz) [%d]\n",
           AVAILABLE_FREQ, DEFAULT_FREQ);
   fprintf(stderr,
           "       -g g  : gain (..,-2,-1,0,1,2,..)                    [%d]\n",
           GAIN);
   fprintf(stderr,
           "       -a a  : amplitude gain (0..100)                     [N/A]\n");
#ifdef SPARC
   fprintf(stderr,
           "       -o o  : output port                                 [%c]\n",
           OUTPORT);
   fprintf(stderr, "                  s (speaker)    h (headphone)\n");
#endif                          /* SPARC */
   fprintf(stderr,
           "       -H H  : header size in byte                         [%d]\n",
           HEADERSIZE);
   fprintf(stderr,
           "       -v    : display filename                            [%s]\n",
           BOOL_STR[VERBOSE]);
   fprintf(stderr,
           "       -w    : byteswap                                    [FALSE]\n");
   fprintf(stderr,
           "       +x    : data format                                 [s]\n");
   fprintf(stderr, "                  s (short)  f (float)   d (double)\n");
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr,
           "       data                                                [stdin]\n");
   fprintf(stderr, "  notice:\n");
   fprintf(stderr, "       number of infile < %d\n", MAXFILES);
#ifdef PACKAGE_VERSION
   fprintf(stderr, "\n");
   fprintf(stderr, " SPTK: version %s\n", PACKAGE_VERSION);
   fprintf(stderr, " CVS Info: %s", rcs_id);
#endif                          /* PACKAGE_VERSION */
   fprintf(stderr, "\n");
   exit(status);
}

static char outport = OUTPORT;
static short *y = NULL, *xs;
static int gain = GAIN, is_verbose = VERBOSE;
static int hdr_size = HEADERSIZE;
static size_t data_size = sizeof(short);
static int freq = DEFAULT_FREQ * 1000;
static float *xf;
static double *x, fgain = 1.0;
double ampgain = -1.0;
int byteswap = 0;
size_t abuf_size;

#if defined(LINUX) || defined(FreeBSD)
int org_vol, org_channels, org_precision, org_freq;
#endif                          /* LINUX or FreeBSD */

#if defined(SOLARIS) || defined(SUNOS)
audio_info_t org_data;
#endif                          /* SOLARIS or SUNOS */

int main(int argc, char *argv[])
{
   FILE *fp;
   char *s, *infile[MAXFILES], c;
   int i, nfiles = 0;

   void direct(FILE *);
   void sndinit(void);
   void init_audiodev(int dtype);
   void reset_audiodev(void);

   if ((s = getenv("DA_FLOAT")) != NULL)
      data_size = sizeof(float);
   if ((s = getenv("DA_DOUBLE")) != NULL)
      data_size = sizeof(double);
   if ((s = getenv("DA_SMPLFREQ")) != NULL)
      freq = (int) (1000 * atof(s));
   if ((s = getenv("DA_GAIN")) != NULL)
      gain = atoi(s) + INITGAIN;
   if ((s = getenv("DA_AMPGAIN")) != NULL)
      ampgain = atof(s);
   if ((s = getenv("DA_PORT")) != NULL)
      outport = *s;
   if ((s = getenv("DA_HDRSIZE")) != NULL)
      hdr_size = atoi(s);

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;

   while (--argc)
      if (*(s = *++argv) == '-') {
         c = *++s;
         switch (c) {
         case 's':
            freq = (int) (1000 * atof(*++argv));
            --argc;
            break;
         case 'g':
            gain = atoi(*++argv) + INITGAIN;
            --argc;
            break;
         case 'a':
            ampgain = atof(*++argv);
            --argc;
            break;
         case 'H':
            hdr_size = atoi(*++argv);
            --argc;
            break;
         case 'v':
            is_verbose = 1 - is_verbose;
            break;
         case 'w':
            byteswap = 1;
            break;
         case 'o':
            outport = **++argv;
            --argc;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else if (*s == '+') {
         c = *++s;
         switch (c) {
         case 's':
            data_size = sizeof(short);
            break;
         case 'f':
            data_size = sizeof(float);
            break;
         case 'd':
            data_size = sizeof(double);
            break;
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      } else {
         if (nfiles < MAXFILES)
            infile[nfiles++] = s;
         else {
            fprintf(stderr, "%s : Number of files exceed %d!\n", cmnd,
                    MAXFILES);
            return (1);
         }
      }

   x = dgetmem(SIZE);
   xf = (float *) x;
   xs = (short *) x;
   y = sgetmem(2 * SIZE);

   sndinit();
   i = (gain < 0) ? -gain : gain;
   while (i--)
      fgain *= 2.0;
   if (gain < 0)
      fgain = 1.0 / fgain;

   if (nfiles) {
      for (i = 0; i < nfiles; i++) {
         fp = getfp(infile[i], "rb");
         if (is_verbose) {
            fprintf(stderr, "%s : %s\n", cmnd, infile[i]);
         }
         direct(fp);
         fclose(fp);
      }
   } else
      direct(stdin);

#ifndef WIN32
   fclose(adfp);
   close(ACFD);
#endif

   reset_audiodev();

   return 0;
}

void direct(FILE * fp)
{
   int k, nread;
   double d;
   void sndout(int leng);
   int byteswap_vec(void *vec, int size, int blocks);

   if (hdr_size)
      fseek(fp, (long) hdr_size, 0);

   while ((nread = freadx(x, data_size, SIZE, fp))) {
      for (k = 0; k < nread; k++) {
         if (data_size == sizeof(double))
            d = x[k];
         else if (data_size == sizeof(float))
            d = *(xf + k);
         else
            d = *(xs + k);
         y[k] = d * fgain;
      }
      if (byteswap > 0)
         byteswap_vec(y, sizeof(short), nread);
      sndout(nread);
   }
}

void sndinit(void)
{
   int dtype;
   void init_audiodev(int dtype);
   void change_output_port(unsigned int port);
   void change_play_gain(float volume);

   switch (freq) {
   case 8000:
      dtype = _8000_16BIT_LINEAR;
      break;
   case 11000:
      dtype = _11025_16BIT_LINEAR;
      break;
   case 11025:
      dtype = _11025_16BIT_LINEAR;
      break;
   case 16000:
      dtype = _16000_16BIT_LINEAR;
      break;
   case 22000:
      dtype = _22050_16BIT_LINEAR;
      break;
   case 22050:
      dtype = _22050_16BIT_LINEAR;
      break;
   case 32000:
      dtype = _32000_16BIT_LINEAR;
      break;
   case 44000:
      dtype = _44100_16BIT_LINEAR;
      break;
   case 44100:
      dtype = _44100_16BIT_LINEAR;
      break;
   case 48000:
      dtype = _48000_16BIT_LINEAR;
      break;
   default:
      fprintf(stderr, "%s : Unavailable sampling frequency %d!\n", cmnd, freq);
      exit(1);
   }
   init_audiodev(dtype);


   if (ampgain >= 0.0) {
      if (ampgain > 100.0)
         ampgain = 100.0;
      change_play_gain(ampgain);
   }
#ifdef SPARC
   int port;
   if (outport == 's')
      port = SPEAKER;
   else if (outport == 'h')
      port = HEADPHONE;
   change_output_port(port | LINE_OUT);
#endif                          /* SPARC */
}

void sndout(int leng)
{
#ifdef WIN32
   win32_audio_play(y, leng);
#else
   fwritex(y, sizeof(short), leng, adfp);
   write(ADFD, y, 0);
#endif
}



void init_audiodev(int dtype)
{
#if defined(LINUX) || defined(FreeBSD)
   int arg;

   adfp = getfp(AUDIO_DEV, "wb");
#ifdef LINUX
   ADFD = adfp->_fileno;
#else                           /* FreeBSD */
   ADFD = adfp->_file;
#endif
   ACFD = open(MIXER_DEV, O_RDWR, 0);

   ioctl(ADFD, SNDCTL_DSP_GETBLKSIZE, &abuf_size);
   ioctl(ADFD, SNDCTL_DSP_SETFMT, &org_precision);
   ioctl(ADFD, SNDCTL_DSP_CHANNELS, &org_channels);
   ioctl(ADFD, SNDCTL_DSP_SPEED, &org_freq);
   ioctl(ACFD, SOUND_MIXER_READ_PCM, &org_vol);

   arg = data_type[dtype].precision;
   ioctl(ADFD, SNDCTL_DSP_SETFMT, &arg);
   /* arg = data_type[dtype].channel; */
   arg = 0;
   ioctl(ADFD, SNDCTL_DSP_CHANNELS, &arg);
   arg = data_type[dtype].sample;
   ioctl(ADFD, SNDCTL_DSP_SPEED, &arg);
#endif                          /* LINUX || FreeBSD */

#ifdef SPARC
   audio_info_t data;

   ACFD = open(AUDIO_CTLDEV, O_RDWR, 0);
   adfp = getfp(AUDIO_DEV, "wb");
   ADFD = adfp->_file;

   AUDIO_INITINFO(&data);
   ioctl(ACFD, AUDIO_GETINFO, &data);
   bcopy(&data, &org_data, sizeof(audio_info_t));

   data.play.sample_rate = data_type[dtype].sample;
   data.play.precision = data_type[dtype].precision;
   data.play.encoding = data_type[dtype].encoding;

   ioctl(ADFD, AUDIO_SETINFO, &data);
#endif                          /* SPARC */

#ifdef WIN32
   if (WIN32AUDIO_NO_ERROR != win32_audio_open(freq, 16)) {
      fprintf(stderr, "Failed to open win32 audio device\n");
      exit(1);
   }
#endif                          /* WIN32 */
}

void change_output_port(unsigned int port)
{
#ifdef LINUX

#endif                          /* LINUX */

#ifdef SPARC
   audio_info_t data;

   AUDIO_INITINFO(&data);
   ioctl(ACFD, AUDIO_GETINFO, &data);

   data.play.port = port;

   ioctl(ACFD, AUDIO_SETINFO, &data);
#endif                          /* SPARC */

#ifdef WIN32

#endif                          /* WIN32 */
}

void change_play_gain(float volume)
{
   int vol, arg;

#if defined(LINUX) || defined(FreeBSD)
   vol = (int) ((MAXAMPGAIN * volume) / 100);

   arg = vol | (vol << 8);
   ioctl(ACFD, MIXER_WRITE(SOUND_MIXER_PCM), &arg);
#endif                          /* LINUX */

#ifdef SPARC
   audio_info_t data;

   vol = (int) ((MAXAMPGAIN * volume) / 100);
   AUDIO_INITINFO(&data);
   ioctl(ACFD, AUDIO_GETINFO, &data);

   data.play.gain = vol;

   ioctl(ACFD, AUDIO_SETINFO, &data);
#endif                          /* SPARC */

#ifdef WIN32
   vol = (int) ((MAXAMPGAIN * volume) / 100.0);
   win32_audio_set_volume(vol);
#endif                          /* WIN32 */
}

void reset_audiodev(void)
{
#if defined(LINUX) || defined(FreeBSD)
   ACFD = open(MIXER_DEV, O_RDWR, 0);
   ADFD = open(AUDIO_DEV, O_RDWR, 0);

   ioctl(ADFD, SNDCTL_DSP_SETFMT, &org_precision);
   ioctl(ADFD, SNDCTL_DSP_CHANNELS, &org_channels);
   ioctl(ADFD, SNDCTL_DSP_SPEED, &org_freq);
   ioctl(ACFD, SOUND_MIXER_WRITE_PCM, &org_vol);

   close(ADFD);
   close(ACFD);
#endif                          /* LINUX or FreeBSD */

#ifdef SPARC
   ACFD = open(AUDIO_CTLDEV, O_RDWR, 0);
   ioctl(ACFD, AUDIO_SETINFO, &org_data);
   close(ACFD);
#endif                          /* SPARC */

#ifdef WIN32
   win32_audio_close();
#endif                          /* WINDOWS */
}

int byteswap_vec(void *vec, int size, int blocks)
{
   char *q;
   char t;
   int i, j;

   q = (char *) vec;
   for (i = 0; i < blocks; i++) {
      for (j = 0; j < (size / 2); j++) {
         t = *(q + j);
         *(q + j) = *(q + (size - 1 - j));
         *(q + (size - 1 - j)) = t;
      }
      q += size;
   }

   return i;                    /* number of blocks */
}
