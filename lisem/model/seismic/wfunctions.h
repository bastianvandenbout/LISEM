#pragma once

#include "defines.h"
#include <math.h>   
#define RAND48_SEED_0   (0x330e)
#define RAND48_SEED_1 (0xabcd)
#define RAND48_SEED_2 (0x1234)
#define RAND48_MULT_0 (0xe66d)
#define RAND48_MULT_1 (0xdeec)
#define RAND48_MULT_2 (0x0005)
#define RAND48_ADD (0x000b)

static unsigned short _rand48_seed[3] = {
        RAND48_SEED_0,
         RAND48_SEED_1,
         RAND48_SEED_2
};
static unsigned short _rand48_mult[3] = {
         RAND48_MULT_0,
         RAND48_MULT_1,
         RAND48_MULT_2
 };
static unsigned short _rand48_add = RAND48_ADD;

static void
 _dorand48(unsigned short xseed[3])
 {
	         unsigned long accu;
	         unsigned short temp[2];
	
	         accu = (unsigned long)_rand48_mult[0] * (unsigned long)xseed[0] +
	          (unsigned long)_rand48_add;
	         temp[0] = (unsigned short)accu;        /* lower 16 bits */
	         accu >>= sizeof(unsigned short)* 8;
	         accu += (unsigned long)_rand48_mult[0] * (unsigned long)xseed[1] +
	          (unsigned long)_rand48_mult[1] * (unsigned long)xseed[0];
	         temp[1] = (unsigned short)accu;        /* middle 16 bits */
	         accu >>= sizeof(unsigned short)* 8;
	         accu += _rand48_mult[0] * xseed[2] + _rand48_mult[1] * xseed[1] + _rand48_mult[2] * xseed[0];
	         xseed[0] = temp[0];
	         xseed[1] = temp[1];
	         xseed[2] = (unsigned short)accu;
}

#ifdef WIN32
static double erand48(unsigned short xseed[3])
{
         _dorand48(xseed);
         return ldexp((double) xseed[0], -48) +
                ldexp((double) xseed[1], -32) +
                ldexp((double) xseed[2], -16);
}

static double drand48(){
	return erand48(_rand48_seed);
}
static void srand48(long seed){
	_rand48_seed[0] = RAND48_SEED_0;
	_rand48_seed[1] = (unsigned short)seed;
	_rand48_seed[2] = (unsigned short)(seed >> 16);
	_rand48_mult[0] = RAND48_MULT_0;
	_rand48_mult[1] = RAND48_MULT_1;
	_rand48_mult[2] = RAND48_MULT_2;
	_rand48_add = RAND48_ADD;
}


#endif

/* Emulate fsync on platforms that lack it, primarily Windows and
   cross-compilers like MinGW.

   This is derived from sqlite3 sources.
   http://www.sqlite.org/cvstrac/rlog?f=sqlite/src/os_win.c
   http://www.sqlite.org/copyright.html

   Written by Richard W.M. Jones <rjones.at.redhat.com>

   Copyright (C) 2008-2012 Free Software Foundation, Inc.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <unistd.h>

#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__

/* FlushFileBuffers */
# define WIN32_LEAN_AND_MEAN
# include <windows.h>

# include <errno.h>


static int
fsync (int fd)
{
  HANDLE h = (HANDLE) _get_osfhandle (fd);
  DWORD err;

  if (h == INVALID_HANDLE_VALUE)
    {
      errno = EBADF;
      return -1;
    }

  if (!FlushFileBuffers (h))
    {
      /* Translate some Windows errors into rough approximations of Unix
       * errors.  MSDN is useless as usual - in this case it doesn't
       * document the full range of errors.
       */
      err = GetLastError ();
      switch (err)
        {
        case ERROR_ACCESS_DENIED:
          /* For a read-only handle, fsync should succeed, even though we have
             no way to sync the access-time changes.  */
          return 0;

          /* eg. Trying to fsync a tty. */
        case ERROR_INVALID_HANDLE:
          errno = EINVAL;
          break;

        default:
          errno = EIO;
        }
      return -1;
    }

  return 0;
}

#else /* !Windows */

#include <unistd.h>
//# error "This platform lacks fsync function, and Gnulib doesn't provide a replacement. This is a bug in Gnulib."

#endif /* !Windows */
